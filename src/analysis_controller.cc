// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "analysis_controller.h"

#include "appcommon/widgets/status_bar_log.h"
#include "appcommon/widgets/timer.h"
#include "models/base/helper.h"
#include "wx/wx.h"

#include "on_sag_app.h"

AnalysisThread::AnalysisThread() : wxThread(wxTHREAD_JOINABLE) {
  span_ = nullptr;
}

void AnalysisThread::AddAnalysisJob(AnalysisJob* job) {
  jobs_.push_back(job);
}

const SagSpan* AnalysisThread::span() const {
  return span_;
}

const std::list<ErrorMessage>* AnalysisThread::messages() const {
  return &messages_;
}

const SpanSagger* AnalysisThread::sagger() const {
  return &sagger_;
}

void AnalysisThread::set_span(const SagSpan* span) {
  span_ = span;
}

void AnalysisThread::DoAnalysisJob(const int& index) {
  std::string message;
  AnalysisJob* job = *std::next(jobs_.begin(), index);

  // sets up sagger for job
  sagger_.set_temperature(job->temperature);

  // validates sagger and logs any errors
  std::list<ErrorMessage> messages;
  const bool status_sagger = sagger_.Validate(false, &messages);
  if (status_sagger == false) {
    // errors were present
    // adds analysis controller error message to give context
    ErrorMessage message;
    message.title = "ANALYSIS THREAD";
    message.description = "No sagging solution for "
                          + helper::DoubleToFormattedString(*job->temperature,
                                                            0)
                          + " degrees";
    messages_.push_back(message);

    // adds sagger error messages
    messages_.splice(messages_.cend(), messages);
  }

  // populates result
  SaggingAnalysisResult& result = *job->result;

  result.angle_transit = -999999;
  result.catenary = Catenary3d();
  result.direction_transit = AxisDirectionType::kNull;
  result.distance_target = -999999;
  result.factor_control = -999999;
  result.offset_coordinates = Point2d<double>();
  result.point_target = Point2d<double>();
  result.temperature_cable = nullptr;
  result.tension_dyno = -999999;
  result.time_stopwatch = -999999;

  // caches result if no errors were present
  if (status_sagger == true) {
    result.catenary = sagger_.Catenary();
    result.offset_coordinates = span_->structure_back.point_attachment;
    result.temperature_cable = job->temperature;

    // selects result based on method type
    if (sagger_.method()->type == SagMethod::Type::kDynamometer) {
      result.tension_dyno = sagger_.TensionDyno();
    } else if (sagger_.method()->type == SagMethod::Type::kStopWatch) {
      result.time_stopwatch = sagger_.TimeStopwatch();
    } else if (sagger_.method()->type == SagMethod::Type::kTransit) {
      result.factor_control = sagger_.FactorControl();
      result.angle_transit = sagger_.AngleTransit();
      result.direction_transit = sagger_.DirectionTransit();
      result.point_target = sagger_.PointTarget();
      result.distance_target = sagger_.DistanceAttachmentToTarget();
    }
  }
}

wxThread::ExitCode AnalysisThread::Entry() {
  // initializes sagger
  sagger_.set_cable(&span_->cable);
  sagger_.set_method(&span_->method);
  sagger_.set_structure_ahead(&span_->structure_ahead);
  sagger_.set_structure_back(&span_->structure_back);
  sagger_.set_units(wxGetApp().config()->units);

  // does all jobs in the list
  const int kSizeJobs = jobs_.size();
  for (int i = 0; i < kSizeJobs; i++) {
    DoAnalysisJob(i);
  }

  // exits
  return (wxThread::ExitCode)0;
}


AnalysisController::AnalysisController() {
  span_ = nullptr;

  max_threads_ = wxThread::GetCPUCount();
  if (max_threads_ == -1) {
    max_threads_ = 1;
  }
}

AnalysisController::~AnalysisController() {
}

void AnalysisController::ClearResults() {
  results_.clear();

  status_bar_log::SetText("Ready", 0);
}

const SaggingAnalysisResult* AnalysisController::Result(
    const int& index) const {
  // checks index
  const int kSizeResults = results_.size();
  if ((index < 0) || (kSizeResults <= index)) {
    return nullptr;
  }

  // checks if result is valid before returning pointer
  if (results_.at(index).temperature_cable == nullptr) {
    return nullptr;
  } else {
    return &results_.at(index);
  }
}

const std::vector<SaggingAnalysisResult>* AnalysisController::Results() const {
  return &results_;
}

void AnalysisController::RunAnalysis() {
  std::string message;

  // clears cached results
  ClearResults();

  // checks if span has been selected
  if (span_ == nullptr) {
    wxLogVerbose("No span is selected. Aborting analysis.");
    return;
  }

  // validates span
  std::list <ErrorMessage> errors;
  if (span_->Validate(false, &errors) == false) {
    // logs errors
    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      std::string message = "Span: " + span_->description + "  --  "
                + error.description;
      wxLogError(message.c_str());
    }

    wxLogError("Span validation errors are present. Aborting analysis.");
    status_bar_log::SetText("Span validation error(s) present, see logs", 0);

    return;
  }

  // starts analysis timer
  Timer timer;
  timer.Start();

  // updates analysis temperatures
  UpdateTemperatures();

  // creates empty set of results that will be populated by worker threads
  results_.resize(temperatures_.size(), SaggingAnalysisResult());

  // creates a job list
  std::list<AnalysisJob> jobs;
  for (auto iter = temperatures_.cbegin(); iter != temperatures_.cend();
       iter++) {
    const int index = std::distance(temperatures_.cbegin(), iter);

    AnalysisJob job;
    job.temperature = &(*iter);
    job.result = &results_[index];
    jobs.push_back(job);
  }

  // determines the number of analysis threads to use
  int num_threads = -1;
  const int num_jobs = jobs.size();

  if (num_jobs < max_threads_) {
    num_threads = num_jobs;
  } else {
    num_threads = max_threads_;
  }

  // creates analysis threads
  std::list<AnalysisThread*> threads;
  for (int i = 0; i < num_threads; i++) {
    AnalysisThread* thread = new AnalysisThread();
    thread->set_span(span_);
    threads.push_back(thread);
  }

  // adds jobs to threads round robin style
  auto iter_jobs = jobs.begin();
  while (iter_jobs != jobs.end()) {
    for (auto iter = threads.begin(); iter != threads.end(); iter++) {
      AnalysisThread* thread = *iter;

      if (iter_jobs == jobs.cend()) {
        break;
      } else {
        AnalysisJob* job = &(*iter_jobs);
        thread->AddAnalysisJob(job);

        iter_jobs++;
      }
    }
  }

  // logs analysis start
  message = "Calculating " + std::to_string(num_jobs)
            + " sagging solutions using "
            + std::to_string(num_threads) + " threads.";
  wxLogVerbose(message.c_str());
  status_bar_log::PushText("Running sagging analysis...", 0);

  // starts threads
  for (auto iter = threads.begin(); iter != threads.end(); iter++) {
    AnalysisThread* thread = *iter;
    wxThreadError status = thread->Run();
    if (status != wxTHREAD_NO_ERROR) {
      wxLogError("Couldn't start analysis thread");
    }
  }

  // waits for threads to complete (i.e. joins worker threads back into main)
  bool is_errors = false;
  for (auto iter_thread = threads.begin(); iter_thread != threads.end();
       iter_thread++) {
    AnalysisThread* thread = *iter_thread;
    thread->Wait();

    // collects any thread errors and logs
    const std::list<ErrorMessage>* messages = thread->messages();
    for (auto iter_message = messages->cbegin();
         iter_message != messages->cend(); iter_message++) {
      is_errors = true;
      const ErrorMessage& message_error = *iter_message;
      std::string str = message_error.title + " - " + message_error.description;
      wxLogError(str.c_str());
    }
  }

  // deletes threads
  for (auto iter = threads.begin(); iter != threads.end(); iter++) {
    AnalysisThread* thread = *iter;
    delete thread;
  }

  // stops timer and logs
  timer.Stop();
  message = "Analysis time = "
            + helper::DoubleToFormattedString(timer.Duration(), 3) + "s.";
  wxLogVerbose(message.c_str());

  // notifies user of any errors
  if (is_errors == true) {
    // notifies user of error
    message = "Analysis encountered error(s). Check logs.";
    wxMessageBox(message);
  }

  // clears status bar
  status_bar_log::PopText(0);
  status_bar_log::SetText("Ready", 0);
}

void AnalysisController::set_span(const SagSpan* span) {
  span_ = span;
}

const SagSpan* AnalysisController::span() const {
  return span_;
}

bool AnalysisController::UpdateTemperatures() {
  temperatures_.clear();

  // gets the lowest temperature
  const double temperature_low = span_->temperature_base
                                 - (2 * span_->temperature_interval);

  // calculates 5 target temperatures
  for (int i = 0; i <= 4; i++) {
    const double temperature = temperature_low
                               + (i * span_->temperature_interval);
    temperatures_.push_back(temperature);
  }

  return true;
}

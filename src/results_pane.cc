// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/results_pane.h"

#include "models/base/helper.h"

#include "onsag/on_sag_app.h"
#include "onsag/on_sag_doc.h"
#include "onsag/on_sag_view.h"

BEGIN_EVENT_TABLE(ResultsPane, wxPanel)
  EVT_LIST_ITEM_SELECTED(wxID_ANY, ResultsPane::OnListCtrlSelect)
END_EVENT_TABLE()

ResultsPane::ResultsPane(wxWindow* parent, wxView* view)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL) {
  // saves view reference
  view_ = view;

  // creates a report table
  table_ = new ReportTable(this);
  table_->set_data(&data_);

  // creates a sizer to layout controls
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(table_, 1, wxEXPAND);
  this->SetSizerAndFit(sizer);
}

ResultsPane::~ResultsPane() {
}

void ResultsPane::Update(wxObject* hint) {
  // interprets hint
  const UpdateHint* hint_update = dynamic_cast<UpdateHint*>(hint);
  if (hint_update == nullptr) {
    // do nothing, this is only passed when pane is created
  } else if (hint_update->type() == UpdateHint::Type::kPreferencesEdit) {
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type() == UpdateHint::Type::kSpansEdit) {
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type() == UpdateHint::Type::kViewSelect) {
    // do nothing
  }
}

void ResultsPane::OnListCtrlSelect(wxListEvent& event) {
  // not creating busy cursor to avoid cursor flicker

  wxLogVerbose("Updating displayed result index.");

  // gets selected index
  // this includes any sorting that is applied
  const long index_selected = event.GetItem().GetId();

  // updates report table
  table_->set_index_selected(index_selected);

  // gets selected index with no sorting applied
  // this still may not be the correct index, as invalid results are excluded
  // from the table
  const long index_unsorted = table_->IndexReportRow(index_selected);

  // gets the document index
  const std::string& str_doc = table_->ValueTable(index_unsorted, 0);

  long index_document = -1;
  const OnSagDoc* doc = dynamic_cast<const OnSagDoc*>(view_->GetDocument());
  for (unsigned int i = 0; i < doc->Results()->size(); i++) {
    // gets document result
    const SaggingAnalysisResult* result = doc->Result(i);
    if (result == nullptr) {
      continue;
    }

    // converts result to string
    std::string str_result =
        helper::DoubleToFormattedString(*result->temperature_cable, 0);

    // compares document string to result string
    if (str_doc == str_result) {
      index_document = i;
      break;
    }
  }

  // updates view index
  OnSagView* view = dynamic_cast<OnSagView*>(view_);
  view->set_index_result(index_document);

  // updates views
  UpdateHint hint(UpdateHint::Type::kViewSelect);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void ResultsPane::UpdateReportData() {
  wxLogVerbose("Updating report table data.");

  // initializes data
  data_.headers.clear();
  data_.rows.clear();

  // gets activated span from document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const SagSpan* span = doc->SpanActivated();
  if (span == nullptr) {
    return;
  }

  // gets all valid results from document
  std::list<const SaggingAnalysisResult*> results;
  for (unsigned int i = 0; i < doc->Results()->size(); i++) {
    const SaggingAnalysisResult* result = doc->Result(i);
    if (result != nullptr) {
      results.push_back(result);
    }
  }

  // selects based on report type
  if (span->method.type == SagMethod::Type::kDynamometer) {
    UpdateReportDataDyno(&results);
  } else if (span->method.type == SagMethod::Type::kStopWatch) {
    UpdateReportDataStopwatch(&results);
  } else if (span->method.type == SagMethod::Type::kTransit) {
    UpdateReportDataTransit(&results);
  }

  // resets view filter index if no data is present
  OnSagView* view = dynamic_cast<OnSagView*>(view_);
  if (results.empty() == true) {
    view->set_index_result(-1);
  }
}

void ResultsPane::UpdateReportDataDyno(
    const std::list<const SaggingAnalysisResult*>* results) {
  // fills column headers
  ReportColumnHeader header;
  header.title = "Temperature";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 100;
  data_.headers.push_back(header);

  header.title = "H";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "S";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "T";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SaggingAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    double value;
    std::string str;

    // adds temperature
    value = *result->temperature_cable;
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // adds tension-horizontal
    value = result->catenary.tension_horizontal();
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // adds sag
    value = result->catenary.Sag();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds tension-dyno
    value = result->tension_dyno;
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

void ResultsPane::UpdateReportDataStopwatch(
    const std::list<const SaggingAnalysisResult*>* results) {
  // fills column headers
  ReportColumnHeader header;
  header.title = "Temperature";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 100;
  data_.headers.push_back(header);

  header.title = "H";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "S";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "WS";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "L";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Time";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SaggingAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    double value;
    std::string str;

    // adds temperature
    value = *result->temperature_cable;
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // adds tension-horizontal
    value = result->catenary.tension_horizontal();
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // adds sag
    value = result->catenary.Sag();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds speed-wave
    value = result->speed_wave;
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds length
    value = result->catenary.Length();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds time-stopwatch
    value = result->time_stopwatch;
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

void ResultsPane::UpdateReportDataTransit(
    const std::list<const SaggingAnalysisResult*>* results) {
  // fills column headers
  ReportColumnHeader header;
  header.title = "Temperature";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 100;
  data_.headers.push_back(header);

  header.title = "H";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "S";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "CF";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "A";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "t";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SaggingAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    double value;
    std::string str;

    // adds temperature
    value = *result->temperature_cable;
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // adds tension-horizontal
    value = result->catenary.tension_horizontal();
    str = helper::DoubleToFormattedString(value, 0);
    row.values.push_back(str);

    // adds sag
    value = result->catenary.Sag();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds factor-control
    value = result->factor_control;
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds angle-transit
    value = result->angle_transit;
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds distance from point-attachment to point-target
    value = result->distance_target;
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "on_sag_doc.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/cmdproc.h"
#include "wx/xml/xml.h"

#include "on_sag_app.h"
#include "on_sag_doc_xml_handler.h"
#include "sag_span_unit_converter.h"

IMPLEMENT_DYNAMIC_CLASS(OnSagDoc, wxDocument)

OnSagDoc::OnSagDoc() {
}

OnSagDoc::~OnSagDoc() {
}

bool OnSagDoc::AppendSpan(const SagSpan& span) {
  spans_.push_back(span);

  Modify(true);

  return true;
}

void OnSagDoc::ConvertUnitStyle(const units::UnitSystem& system,
                                       const units::UnitStyle& style_from,
                                       const units::UnitStyle& style_to) {
  if (style_from == style_to) {
    return;
  }

  // converts spans
  for (auto it = spans_.begin(); it != spans_.end(); it++) {
    SagSpan& span = *it;
    SagSpanUnitConverter::ConvertUnitStyle(system, style_from,
                                           style_to, span);
  }

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();
}

void OnSagDoc::ConvertUnitSystem(const units::UnitSystem& system_from,
                                        const units::UnitSystem& system_to) {
  if (system_from == system_to) {
    return;
  }

  // converts spans
  for (auto it = spans_.begin(); it != spans_.end(); it++) {
    SagSpan& span = *it;
    SagSpanUnitConverter::ConvertUnitSystem(system_from, system_to, span);
  }

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();
}

bool OnSagDoc::DeleteSpan(const int& index) {
  // checks index
  if (IsValidIndex(index, false) == false) {
    return false;
  }

  // deletes from span list
  auto iter = std::next(spans_.begin(), index);
  spans_.erase(iter);

  // marks as modified
  Modify(true);

  // updates activated span index
  if (index == index_activated_) {
    index_activated_ = -1;
  } else if (index < index_activated_) {
    index_activated_--;
  }

  // syncs controller
  SyncAnalysisController();

  return true;
}

int OnSagDoc::IndexSpan(const SagSpan* span) {
  // searches list of spans for a match
  for (auto iter = spans_.cbegin(); iter != spans_.cend(); iter++) {
    const SagSpan* span_doc = &(*iter);
    if (span == span_doc) {
      return std::distance(spans_.cbegin(), iter);
    }
  }

  // if it reaches this point, no match was found
  return -1;
}

bool OnSagDoc::InsertSpan(const int& index, const SagSpan& span) {
  // checks index
  if (IsValidIndex(index, true) == false) {
    return false;
  }

  // inserts span
  auto iter = std::next(spans_.begin(), index);
  spans_.insert(iter, span);

  // marks as modified
  Modify(true);

  // updates activated span index
  if (index < index_activated_) {
    index_activated_++;
  }

  // syncs controller
  SyncAnalysisController();

  return true;
}

bool OnSagDoc::IsUniqueDescription(const std::string& description) const {
  // searches all spans for a match
  for (auto iter = spans_.cbegin(); iter != spans_.cend(); iter++) {
    const SagSpan& span = *iter;

    // compares and breaks if name is not unique
    if (span.description == description) {
      return false;
    }
  }

  // if it makes it to this point, the name is unique
  return true;
}

bool OnSagDoc::IsValidIndex(const int& index,
                                   const bool& is_included_end) const {
  const int kSizeSpans = spans_.size();
  if ((0 <= index) && (index < kSizeSpans)) {
    return true;
  } else if ((index == kSizeSpans) && (is_included_end == true)) {
    return true;
  } else {
    return false;
  }
}

wxInputStream& OnSagDoc::LoadObject(wxInputStream& stream) {
  wxBusyCursor cursor;

  std::string message;

  message = "Loading document file: " + this->GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml structure. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalid state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc_xml.GetRoot();
  if (root->GetName() != "on_sag_doc") {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml root. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      // notifies user of error
      message = GetFilename() + "  --  "
                "Document file contains an invalid units attribute. The "
                "document will close.";
      wxLogError(message.c_str());
      wxMessageBox(message);

      status_bar_log::PopText(0);

      // sets stream to invalide state and returns
      stream.Reset(wxSTREAM_READ_ERROR);
      return stream;
    }
  } else {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file is missing units attribute. The document will "
              "close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // parses the XML node and loads into the document
  std::string filename = this->GetFilename();
  const bool status_node = OnSagDocXmlHandler::ParseNode(root, filename, *this);
  if (status_node == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains parsing error(s). Check logs.";
    wxMessageBox(message);
  }

  // converts units to consistent style
  ConvertUnitStyle(units_file,
                   units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    ConvertUnitSystem(units_file, units_config);
  }

  // resets modified status to false because the xml parser uses functions
  // that mark it as modified
  Modify(false);

  status_bar_log::PopText(0);

  return stream;
}

bool OnSagDoc::ModifySpan(const int& index, const SagSpan& span) {
  // checks index
  if (IsValidIndex(index, false) == false) {
    return false;
  }

  // modifies span in list
  auto iter = std::next(spans_.begin(), index);
  *iter = SagSpan(span);

  // sets document flag as modified
  Modify(true);

  // runs analysis if necessary
  if (index == index_activated_) {
    controller_analysis_.RunAnalysis();
  }

  return true;
}

bool OnSagDoc::MoveSpan(const int& index_from, const int& index_to) {
  // checks indexes
  if (IsValidIndex(index_from, false) == false) {
    return false;
  }

  if (IsValidIndex(index_to, true) == false) {
    return false;
  }

  // gets an iterator to the activated span
  std::list<SagSpan>::const_iterator iter_activated;
  if (index_activated_ != -1) {
    iter_activated = std::next(spans_.cbegin(), index_activated_);
  }

  // moves span in list
  auto iter_from = std::next(spans_.begin(), index_from);
  auto iter_to = std::next(spans_.begin(), index_to);

  spans_.splice(iter_to, spans_, iter_from);

  // marks as modified
  Modify(true);

  // updates activated index
  if (index_activated_ != -1) {
    index_activated_ = std::distance(spans_.cbegin(), iter_activated);
  }

  // syncs controller
  SyncAnalysisController();

  return true;
}

bool OnSagDoc::OnCreate(const wxString& path, long flags) {
  // initializes activated span
  index_activated_ = -1;

  // calls base class function
  return wxDocument::OnCreate(path, flags);
}

const SaggingAnalysisResult* OnSagDoc::Result(const int& index) const {
  return controller_analysis_.Result(index);
}

const std::vector<SaggingAnalysisResult>* OnSagDoc::Results() const {
  return controller_analysis_.Results();
}

void OnSagDoc::RunAnalysis() const {
  controller_analysis_.RunAnalysis();
}

wxOutputStream& OnSagDoc::SaveObject(wxOutputStream& stream) {
  wxBusyCursor cursor;

  // logs
  std::string message = "Saving document file: " + GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // gets the unit system from app config
  units::UnitSystem units = wxGetApp().config()->units;

  // converts to a different unit style for saving
  ConvertUnitStyle(units, units::UnitStyle::kConsistent,
                   units::UnitStyle::kDifferent);

  // generates an xml node
  wxXmlNode* root = OnSagDocXmlHandler::CreateNode(*this, units);

  // adds unit attribute to xml node
  // this attribute should be added at this step vs the xml handler because
  // the attribute describes all values in the file, and is consistent
  // with how the FileHandler functions work
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an XML document and saves to stream
  wxXmlDocument doc_xml;
  doc_xml.SetRoot(root);
  doc_xml.Save(stream);

  // converts back to a consistent unit style
  ConvertUnitStyle(units, units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();

  status_bar_log::PopText(0);

  return stream;
}

const SagSpan* OnSagDoc::SpanActivated() const {
  return controller_analysis_.span();
}

int OnSagDoc::index_activated() const {
  return index_activated_;
}

bool OnSagDoc::set_index_activated(const int& index) {
  // checks if span is to be deactivated
  if (index == -1) {
    index_activated_ = index;
    SyncAnalysisController();
    return true;
  }

  // checks index
  if (IsValidIndex(index, false) == false) {
    return false;
  }

  // updates activated index
  index_activated_ = index;

  // syncs controller
  SyncAnalysisController();

  return true;
}

const std::list<SagSpan>& OnSagDoc::spans() const {
  return spans_;
}

void OnSagDoc::SyncAnalysisController() {
  // exits if no span is activated
  if (index_activated_ == -1) {
    controller_analysis_.set_span(nullptr);
    controller_analysis_.ClearResults();
    return;
  }

  // gets a pointer to the activated span
  const SagSpan* span = &(*std::next(spans_.cbegin(), index_activated_));

  // forces controller to update if spans don't match
  if (span != controller_analysis_.span()) {
    controller_analysis_.set_span(span);
    controller_analysis_.RunAnalysis();
  }
}

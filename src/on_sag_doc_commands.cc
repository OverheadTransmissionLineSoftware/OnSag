// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/on_sag_doc_commands.h"

#include "wx/wx.h"

#include "onsag/on_sag_app.h"
#include "onsag/on_sag_doc.h"
#include "onsag/sag_span_xml_handler.h"

// These are the string definitions for the various command types.
const std::string SagSpanCommand::kNameDelete = "Delete Span";
const std::string SagSpanCommand::kNameInsert = "Insert Span";
const std::string SagSpanCommand::kNameModify = "Modify Span";
const std::string SagSpanCommand::kNameMoveDown = "Move Span Down";
const std::string SagSpanCommand::kNameMoveUp = "Move Span Up";

SagSpanCommand::SagSpanCommand(const std::string& name)
    : wxCommand(true, name) {
  doc_ = wxGetApp().GetDocument();
  index_ = -1;
  node_do_ = nullptr;
  node_undo_ = nullptr;
}

SagSpanCommand::~SagSpanCommand() {
  delete node_do_;
  delete node_undo_;
}

bool SagSpanCommand::CreateSpanFromXml(const wxXmlNode* node, SagSpan& span) {
  // clears span
  span = SagSpan();

  // parses node into span
  const bool status_node = SagSpanXmlHandler::ParseNode(node, "", span);
  if (status_node == false) {
    wxString message =
        SagSpanXmlHandler::FileAndLineNumber("", node)
        + "Invalid span. Skipping.";
    wxLogError(message);
  }

  return status_node;
}

bool SagSpanCommand::Do() {
  bool status = false;

  // clears undo node so it's ready to cache the existing state
  if (node_undo_ != nullptr) {
    delete node_undo_;
  }

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameDelete) {
    // caches span to xml and then does command
    if (doc_->IsValidIndex(index_, false) == false) {
      wxLogError("Invalid span index. Aborting command.");
      return false;
    }
    const SagSpan& span = *std::next(doc_->spans().cbegin(), index_);
    node_undo_ = SaveSpanToXml(span);
    status = DoDelete();
  } else if (name == kNameInsert) {
    // does command
    status = DoInsert(node_do_);
  } else if (name == kNameModify) {
    // caches span to xml and then does command
    if (doc_->IsValidIndex(index_, false) == false) {
      wxLogError("Invalid span index. Aborting command.");
      return false;
    }
    const SagSpan& span = *std::next(doc_->spans().cbegin(), index_);
    node_undo_ = SaveSpanToXml(span);
    status = DoModify(node_do_);
  } else if (name == kNameMoveDown) {
    // does command
    status = DoMoveDown();
  } else if (name == kNameMoveUp) {
    // does command
    status = DoMoveUp();
  } else {
    status = false;

    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // checks if command succeeded
  if (status == true) {
    // posts a view update
    UpdateHint hint(UpdateHint::Type::kSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  } else {
    // logs error
    wxLogError("Do command failed.");
  }

  return status;
}

wxXmlNode* SagSpanCommand::SaveSpanToXml(const SagSpan& span) {
  // copies span
  SagSpan span_modified = span;

  // gets unit system and returns xml node
  units::UnitSystem units = wxGetApp().config()->units;
  return SagSpanXmlHandler::CreateNode(span_modified, "", units);;
}

bool SagSpanCommand::Undo() {
  bool status = false;

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameDelete) {
    status = DoInsert(node_undo_);
  } else if (name == kNameInsert) {
    status = DoDelete();
  } else if (name == kNameModify) {
    status = DoModify(node_undo_);
  } else if (name == kNameMoveDown) {
    status = DoMoveUp();
  } else if (name == kNameMoveUp) {
    status = DoMoveDown();
  } else {
    status = false;

    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // checks if command succeeded
  if (status == true) {
    // posts a view update
    UpdateHint hint(UpdateHint::Type::kSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  } else {
    // logs error
    wxLogError("Undo command failed.");
  }

  return status;
}

int SagSpanCommand::index() const {
  return index_;
}

const wxXmlNode* SagSpanCommand::node_span() const {
  return node_do_;
}

void SagSpanCommand::set_index(const int& index) {
  index_ = index;
}

void SagSpanCommand::set_node_span(const wxXmlNode* node_span) {
  node_do_ = node_span;
}

bool SagSpanCommand::DoDelete() {
  // checks index
  if (doc_->IsValidIndex(index_, false) == false) {
    wxLogError("Invalid index. Aborting delete command.");
    return false;
  }

  // deletes from document
  return doc_->DeleteSpan(index_);
}

bool SagSpanCommand::DoInsert(const wxXmlNode* node) {
  // checks index
  if (doc_->IsValidIndex(index_, true) == false) {
    wxLogError("Invalid index. Aborting insert command.");
    return false;
  }

  // builds span from xml node
  SagSpan span;
  CreateSpanFromXml(node, span);

  // inserts span to document
  return doc_->InsertSpan(index_, span);
}

bool SagSpanCommand::DoModify(const wxXmlNode* node) {
  // checks index
  if (doc_->IsValidIndex(index_, false) == false) {
    wxLogError("Invalid index. Aborting modify command.");
    return false;
  }

  // builds span from xml node
  SagSpan span;
  bool status = CreateSpanFromXml(node, span);
  if (status == false) {
    return false;
  }

  // modifies the document
  return doc_->ModifySpan(index_, span);
}

bool SagSpanCommand::DoMoveDown() {
  // checks index, and checks against the last valid index
  const int kSizeSpans = doc_->spans().size();
  if ((doc_->IsValidIndex(index_, false) == false)
      || (index_ == kSizeSpans - 1)) {
    wxLogError("Invalid index. Aborting move down command.");
    return false;
  }

  // swaps the spans in the document
  bool status = doc_->MoveSpan(index_, index_ + 2);

  // increments the command index
  index_ = index_ + 1;

  return status;
}

bool SagSpanCommand::DoMoveUp() {
  // checks index, and checks against the first valid index
  if ((doc_->IsValidIndex(index_, false) == false)
      || (index_ == 0)) {
    wxLogError("Invalid index. Aborting move up command.");
    return false;
  }

  // swaps the spans in the document
  bool status = doc_->MoveSpan(index_, index_ - 1);

  // decrements the command index
  index_ = index_ - 1;

  return status;
}

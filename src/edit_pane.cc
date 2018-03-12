// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/edit_pane.h"

#include <sstream>

#include "models/base/helper.h"
#include "wx/xrc/xmlres.h"

#include "onsag/on_sag_app.h"
#include "onsag/on_sag_doc.h"
#include "onsag/on_sag_doc_commands.h"
#include "onsag/sag_span_editor_dialog.h"
#include "onsag/sag_span_unit_converter.h"
#include "xpm/copy.xpm"
#include "xpm/minus.xpm"
#include "xpm/move_arrow_down.xpm"
#include "xpm/move_arrow_up.xpm"
#include "xpm/plus.xpm"
#include "xpm/wrench.xpm"

/// context menu enum
enum {
  kTreeItemActivate = 0,
  kTreeItemCopy,
  kTreeItemDeactivate,
  kTreeItemDelete,
  kTreeItemEdit,
  kTreeItemMoveDown,
  kTreeItemMoveUp,
  kTreeRootAdd,
};

BEGIN_EVENT_TABLE(EditPane, wxPanel)
  EVT_BUTTON(XRCID("button_add"), EditPane::OnButtonAdd)
  EVT_BUTTON(XRCID("button_copy"), EditPane::OnButtonCopy)
  EVT_BUTTON(XRCID("button_delete"), EditPane::OnButtonDelete)
  EVT_BUTTON(XRCID("button_edit"), EditPane::OnButtonEdit)
  EVT_BUTTON(XRCID("button_move_down"), EditPane::OnButtonMoveDown)
  EVT_BUTTON(XRCID("button_move_up"), EditPane::OnButtonMoveUp)
  EVT_MENU(wxID_ANY, EditPane::OnContextMenuSelect)
  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, EditPane::OnItemActivate)
  EVT_TREE_ITEM_MENU(wxID_ANY, EditPane::OnItemMenu)
END_EVENT_TABLE()

EditPane::EditPane(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "edit_pane");

  // saves view reference
  view_ = view;

  // sets up treectrl
  treectrl_ = XRCCTRL(*this, "treectrl", wxTreeCtrl);
  treectrl_->AddRoot("Sag Spans");
  treectrl_->SetIndent(2);

  // creates an imagelist for the button bitmaps
  wxImageList images(32, 32, true);
  images.Add(wxBitmap(plus_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(minus_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(copy_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(wrench_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(move_arrow_up_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(move_arrow_down_xpm), wxColour(255, 255, 255));

  // assigns bitmaps to the buttons
  wxButton* button = nullptr;
  button = XRCCTRL(*this, "button_add", wxButton);
  button->SetBitmap(images.GetBitmap(0));

  button = XRCCTRL(*this, "button_delete", wxButton);
  button->SetBitmap(images.GetBitmap(1));

  button = XRCCTRL(*this, "button_copy", wxButton);
  button->SetBitmap(images.GetBitmap(2));

  button = XRCCTRL(*this, "button_edit", wxButton);
  button->SetBitmap(images.GetBitmap(3));

  button = XRCCTRL(*this, "button_move_up", wxButton);
  button->SetBitmap(images.GetBitmap(4));

  button = XRCCTRL(*this, "button_move_down", wxButton);
  button->SetBitmap(images.GetBitmap(5));
}

EditPane::~EditPane() {
}

void EditPane::Update(wxObject* hint) {
  // interprets hint
  const UpdateHint* hint_update = dynamic_cast<UpdateHint*>(hint);
  if (hint_update == nullptr) {
    InitializeTreeCtrl();
  } else if (hint_update->type() == UpdateHint::Type::kPreferencesEdit) {
    // do nothing
  } else if (hint_update->type() == UpdateHint::Type::kSpansEdit) {
    UpdateTreeCtrlSpanItems();
  } else if (hint_update->type() == UpdateHint::Type::kViewSelect) {
    // do nothing
  }
}

void EditPane::ActivateSpan(const wxTreeItemId& id) {
  if (id.IsOk() == false) {
    return;
  }

  if (id == treectrl_->GetRootItem()) {
    return;
  }

  wxLogVerbose("Activating span.");

  // updates document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  auto iter = data->iter();
  const int index = doc->IndexSpan(&(*iter));
  doc->set_index_activated(index);

  // posts a view update
  UpdateHint hint(UpdateHint::Type::kSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);

  // updates treectrl focus
  FocusTreeCtrlSpanItem(index);
}

void EditPane::AddSpan() {
  // gets document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());

  // determines a unique description for the span
  std::string description;
  int i = 1;
  while (true) {
    // adjusts the description name
    description = "New (" + std::to_string(i) + ")";

    // compares against spans in doc
    if (doc->IsUniqueDescription(description) == true) {
      break;
    } else {
      i++;
    }
  }

  // creates new span
  // sets values to zero so it looks less invalid when displayed
  SagSpan span;
  span.description = description;
  span.temperature_base = 0;
  span.temperature_interval = 0;

  span.cable.correction_creep = 0;
  span.cable.correction_sag = 0;
  span.cable.scale = 0;
  span.cable.weight_unit = 0;

  for (int i = 1; i <= 5; i++) {
    SagCable::TensionPoint point;
    point.temperature = 0;
    point.tension_horizontal = 0;

    span.cable.tensions.push_back(point);
  }

  span.structure_back.point_attachment.x = 0;
  span.structure_back.point_attachment.y = 0;

  span.structure_ahead.point_attachment.x = 0;
  span.structure_ahead.point_attachment.y = 0;

  span.method.point_transit.x = 0;
  span.method.point_transit.y = 0;
  span.method.wave_return = 0;

  // creates a span editor dialog
  SagSpanEditorDialog dialog(view_->GetFrame(),
                             wxGetApp().config()->units,
                             &span);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxLogVerbose("Adding span.");

  // updates document
  SagSpanCommand* command = new SagSpanCommand(SagSpanCommand::kNameInsert);
  command->set_index(doc->spans().size());

  wxXmlNode* node = SagSpanCommand::SaveSpanToXml(span);
  command->set_node_span(node);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::CopySpan(const wxTreeItemId& id) {
  // gets document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());

  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  // copies span
  SagSpan span = *(data->iter());

  wxLogVerbose("Copying span.");

  // determines a unique description for the span
  std::string str = span.description;

  // determines is existing description is versioned
  bool is_versioned = true;
  const int pos_end = str.rfind(")", str.size());
  const int pos_start = str.rfind("(", pos_end);
  int version = 1;

  const int kSize = str.size();
  if (pos_end != kSize - 1) {
    // the last character is not a version closing
    is_versioned = false;
  } else if ((str.at(pos_start) == '(') && (str.at(pos_end) == ')')) {
    // extracts the numbers
    const int length = pos_end - pos_start;
    std::string str_extracted = str.substr(pos_start + 1, length - 1);

    // attempts to convert string to integer
    std::stringstream stream(str_extracted);
    stream >> version;

    // checks conversion status
    if (!stream) {
      is_versioned = false;
    }
  }

  // creates base string
  std::string str_base;
  if (is_versioned == false) {
    str_base = str + " ";
  } else {
    str_base = str.substr(0, pos_start);
    //if (str_base.back() == ' ') {
    //  str_base.pop_back();
    //}
  }

  // determines a unique description for the span
  std::string str_version;
  std::string description;
  while (true) {
    // creates version string
    str_version = "(" + std::to_string(version) + ")";

    // combines the base and version names
    description = str_base + str_version;

    // compares against spans in doc
    if (doc->IsUniqueDescription(description) == true) {
      break;
    } else {
      version++;
    }
  }

  // sets the final span description
  span.description = description;

  // updates document
  SagSpanCommand* command = new SagSpanCommand(SagSpanCommand::kNameInsert);
  const int index = std::distance(doc->spans().cbegin(), data->iter()) + 1;
  command->set_index(index);

  wxXmlNode* node = SagSpanCommand::SaveSpanToXml(span);
  command->set_node_span(node);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::DeactivateSpan(const wxTreeItemId& id) {
  wxLogVerbose("Deactivating span.");

  // gets iterator to span
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  auto iter = data->iter();

  // updates document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  doc->set_index_activated(-1);

  // posts a view update
  UpdateHint hint(UpdateHint::Type::kSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);

  // updates treectrl focus
  const int index = std::distance(doc->spans().cbegin(), iter);
  FocusTreeCtrlSpanItem(index);
}

void EditPane::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  wxLogVerbose("Deleting span.");

  // updates document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const int index = std::distance(doc->spans().cbegin(), data->iter());

  SagSpanCommand* command = new SagSpanCommand(SagSpanCommand::kNameDelete);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index() - 1);
}

void EditPane::EditSpan(const wxTreeItemId& id) {
  // gets tree item data and copies span
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  SagSpan span = *(data->iter());

  // converts span to different unit style
  SagSpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                         units::UnitStyle::kConsistent,
                                         units::UnitStyle::kDifferent,
                                         span);

  // creates a span editor dialog
  SagSpanEditorDialog dialog(view_->GetFrame(),
                             wxGetApp().config()->units,
                             &span);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxLogVerbose("Editing span.");

  // converts span to consistent unit style
  SagSpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                         units::UnitStyle::kDifferent,
                                         units::UnitStyle::kConsistent,
                                         span);

  // updates document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());

  SagSpanCommand* command = new SagSpanCommand(SagSpanCommand::kNameModify);
  const int index = std::distance(doc->spans().cbegin(), data->iter());
  command->set_index(index);

  wxXmlNode* node = SagSpanCommand::SaveSpanToXml(span);
  command->set_node_span(node);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::FocusTreeCtrlSpanItem(const int& index) {
  // gets the treectrl item
  wxTreeItemId id;
  wxTreeItemIdValue cookie;
  for (auto i = 0; i <= index; i++) {
    if (i == 0) {
      id = treectrl_->GetFirstChild(treectrl_->GetRootItem(), cookie);
    }
    else {
      id = treectrl_->GetNextSibling(id);
    }
  }

  if (id.IsOk() == true) {
    // sets focus
    treectrl_->SetFocusedItem(id);

    // sets selection
    treectrl_->UnselectAll();
    treectrl_->SelectItem(id);

    // sets application focus
    treectrl_->SetFocus();
  }
}

void EditPane::InitializeTreeCtrl() {
  // gets root
  wxTreeItemId root = treectrl_->GetRootItem();

  // adds items for all spans in document
  UpdateTreeCtrlSpanItems();

  // expands to show all spans
  treectrl_->Expand(root);
}

void EditPane::MoveSpanDown(const wxTreeItemId& id) {
  // checks to make sure item isn't the last one
  wxTreeItemId id_next = treectrl_->GetNextSibling(id);
  if (id_next.IsOk() == false) {
    return;
  }

  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  wxLogVerbose("Moving spans.");

  // updates document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const int index = std::distance(doc->spans().cbegin(), data->iter());

  SagSpanCommand* command = new SagSpanCommand(SagSpanCommand::kNameMoveDown);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::MoveSpanUp(const wxTreeItemId& id) {
  // checks to make sure item isn't the first one
  wxTreeItemId id_prev = treectrl_->GetPrevSibling(id);
  if (id_prev.IsOk() == false) {
    return;
  }

  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  wxLogVerbose("Moving spans.");

  // updates document
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const int index = std::distance(doc->spans().cbegin(), data->iter());

  SagSpanCommand* command = new SagSpanCommand(SagSpanCommand::kNameMoveUp);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::OnButtonAdd(wxCommandEvent& event) {
  // can't create busy cursor, a dialog is used further along

  AddSpan();
}

void EditPane::OnButtonCopy(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // copies span
  CopySpan(id_item);
}

void EditPane::OnButtonDelete(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // deletes span
  DeleteSpan(id_item);
}

void EditPane::OnButtonEdit(wxCommandEvent& event) {
  // can't create busy cursor, a dialog is used further along

  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // edits span
  EditSpan(id_item);
}

void EditPane::OnButtonMoveDown(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // moves span
  MoveSpanDown(id_item);
}

void EditPane::OnButtonMoveUp(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // moves span
  MoveSpanUp(id_item);
}

void EditPane::OnContextMenuSelect(wxCommandEvent& event) {
  // gets selected tree item data
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kTreeItemActivate) {
    wxBusyCursor cursor;
    ActivateSpan(id_item);
  } else if (id_event == kTreeItemCopy) {
    wxBusyCursor cursor;
    CopySpan(id_item);
  } else if (id_event == kTreeItemDeactivate) {
    wxBusyCursor cursor;
    DeactivateSpan(id_item);
  } else if (id_event == kTreeItemDelete) {
    wxBusyCursor cursor;
    DeleteSpan(id_item);
  } else if (id_event == kTreeItemEdit) {
    // can't create busy cursor, a dialog is used further along
    EditSpan(id_item);
  } else if (id_event == kTreeItemMoveDown) {
    wxBusyCursor cursor;
    MoveSpanDown(id_item);
  } else if (id_event == kTreeItemMoveUp) {
    wxBusyCursor cursor;
    MoveSpanUp(id_item);
  } else if (id_event == kTreeRootAdd) {
    // can't create busy cursor, a dialog is used further along
    AddSpan();
  }
}

void EditPane::OnItemActivate(wxTreeEvent& event) {
  wxBusyCursor cursor;

  // gets activated item from event
  wxTreeItemId id = event.GetItem();
  ActivateSpan(id);
}

void EditPane::OnItemMenu(wxTreeEvent& event) {
  // gets item from event
  wxTreeItemId id = event.GetItem();
  treectrl_->SetFocusedItem(id);

  // gets selected span item and the activated span (document)
  // determines if the selected item is currently activated
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  const SagSpan* span_selected = &(*data->iter());

  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const SagSpan* span_activated = doc->SpanActivated();

  bool is_activated = false;
  if (span_selected == span_activated) {
    is_activated = true;
  }

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == treectrl_->GetRootItem()) {
    menu.Append(kTreeRootAdd, "Add Span");
  } else { // a span is selected
    if (is_activated == false) {
      menu.Append(kTreeItemActivate, "Activate");
    } else {
      menu.Append(kTreeItemDeactivate, "Deactivate");
    }
    menu.AppendSeparator();
    menu.Append(kTreeItemEdit, "Edit");
    menu.Append(kTreeItemCopy, "Copy");
    menu.Append(kTreeItemDelete, "Delete");
    menu.AppendSeparator();
    menu.Append(kTreeItemMoveUp, "Move Up");
    menu.Append(kTreeItemMoveDown, "Move Down");
  }

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

/// This method deletes the treectrl items and re-inserts them. The treectrl
/// item focus is not set.
void EditPane::UpdateTreeCtrlSpanItems() {
  treectrl_->Freeze();

  // gets information from document and treectrl
  OnSagDoc* doc = dynamic_cast<OnSagDoc*>(view_->GetDocument());
  const std::list<SagSpan>& spans = doc->spans();
  const SagSpan* span_activated = doc->SpanActivated();

  wxTreeItemId root = treectrl_->GetRootItem();
  treectrl_->DeleteChildren(root);

  // iterates over all spans in the document
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const SagSpan* span = &(*iter);

    // creates treectrl item
    wxTreeItemId item = treectrl_->AppendItem(root, span->description);

    SpanTreeItemData* data = new SpanTreeItemData();
    data->set_iter(iter);
    treectrl_->SetItemData(item, data);

    // adjusts bold for activated and non-activated span
    if (span == span_activated) {
      treectrl_->SetItemBold(item, true);
    } else {
      treectrl_->SetItemBold(item, false);
    }
  }

  treectrl_->Thaw();
}

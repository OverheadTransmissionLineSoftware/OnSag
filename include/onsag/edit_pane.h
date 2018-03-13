// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_EDIT_PANE_H_
#define ONSAG_EDIT_PANE_H_

#include <list>

#include "wx/docview.h"
#include "wx/treectrl.h"
#include "wx/wx.h"

#include "onsag/sag_span.h"

/// \par OVERVIEW
///
/// This class contains the data associated with each item in the treectrl.
/// The data contains a std container iterator that points to where the span is
/// actually stored in the OnSagDoc.
class SpanTreeItemData : public wxTreeItemData {
 public:
  /// \brief Sets the std container iterator for the item.
  /// \param[in] iter
  ///   The iterator.
  void set_iter(std::list<SagSpan>::const_iterator iter) {
    iter_ = iter;
  }

  /// \brief Gets the iterator.
  /// \return The iterator.
  const std::list<SagSpan>::const_iterator iter() {
    return iter_;
  }

 private:
  /// \var iter_
  ///   The std container iterator that points to where the span is stored.
  std::list<SagSpan>::const_iterator iter_;
};


/// \par OVERVIEW
///
/// This class is an wxAUI pane that allows the user to edit the document.
///
/// \par EDITING INTERFACE
///
/// Spans can be modified using the configured buttons, or with a context menu.
///
/// \par COMMANDS
///
/// This class does not edit the document directly. Instead, generates commands
/// and submits them to the command processor.
class EditPane : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  EditPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~EditPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 private:
  /// \brief Activates a span in the treectrl.
  /// \param[in] id
  ///   The item ID of the selected span.
  void ActivateSpan(const wxTreeItemId& id);

  /// \brief Adds a span.
  /// The span will be added to the end.
  void AddSpan();

  /// \brief Copies the selected span.
  /// \param[in] id
  ///   The item ID of the selected span.
  /// The copied span is inserted below the selected span.
  void CopySpan(const wxTreeItemId& id);

  /// \brief Deactivates the span in the treectrl.
  /// \param[in] id
  ///   The item ID of the selected span.
  void DeactivateSpan(const wxTreeItemId& id);

  /// \brief Deletes the selected span.
  /// \param[in] id
  ///   The item ID of the selected span.
  void DeleteSpan(const wxTreeItemId& id);

  /// \brief Edits the selected span.
  /// \param[in] id
  ///   The item ID of the selected span.
  void EditSpan(const wxTreeItemId& id);

  /// \brief Sets the application focus on a treectrl span item.
  /// \param[in] index
  ///   The index of the treectrl span item.
  /// This function will set the treectrl focus to the index and move the
  /// application focus to the treectrl.
  void FocusTreeCtrlSpanItem(const int& index);

  /// \brief Initializes the treectrl.
  /// This function deletes all tree items and re-populates with spans from the
  /// document.
  void InitializeTreeCtrl();

  /// \brief Moves the selected span down.
  /// \param[in] id
  ///   The item ID of the selected span.
  void MoveSpanDown(const wxTreeItemId& id);

  /// \brief Moves the selected span up.
  /// \param[in] id
  ///   The item ID of the selected span.
  void MoveSpanUp(const wxTreeItemId& id);

  /// \brief Handles the add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonAdd(wxCommandEvent& event);

  /// \brief Handles the copy button event.
  /// \param[in] event
  ///   The event.
  void OnButtonCopy(wxCommandEvent& event);

  /// \brief Handles the delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonDelete(wxCommandEvent& event);

  /// \brief Handles the edit button event.
  /// \param[in] event
  ///   The event.
  void OnButtonEdit(wxCommandEvent& event);

  /// \brief Handles the move down button event.
  /// \param[in] event
  ///   The event.
  void OnButtonMoveDown(wxCommandEvent& event);

  /// \brief Handles the move up button event.
  /// \param[in] event
  ///   The event.
  void OnButtonMoveUp(wxCommandEvent& event);

  /// \brief Handles a context menu selection event.
  /// \param[in] event
  ///   The event generated by the context menu. This is used to identify which
  ///   function to call.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles a double click event, which will activate a span.
  /// \param[in] event
  ///   The event.
  void OnItemActivate(wxTreeEvent& event);

  /// \brief Handles the event for a right click, and will generate a context
  ///   menu of options.
  /// \param[in] event
  ///   The event.
  void OnItemMenu(wxTreeEvent& event);

  /// \brief Updates the treectrl.
  void UpdateTreeCtrlSpanItems();

  /// \var view_
  ///   The view.
  wxView* view_;

  /// \var treectrl_
  ///  The treectrl that shows the document.
  wxTreeCtrl* treectrl_;

  DECLARE_EVENT_TABLE()
};

#endif  // ONSAG_EDIT_PANE_H_

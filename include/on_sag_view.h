// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_ONSAGVIEW_H_
#define OTLS_ONSAG_ONSAGVIEW_H_

#include "wx/docview.h"

#include "edit_pane.h"
#include "profile_plot_pane.h"
#include "results_pane.h"

/// \par OVERVIEW
///
/// This is the application view, which is responsible for displaying
/// information and results from the document.
class OnSagView : public wxView {
 public:
  /// \par OVERVIEW
  ///
  /// This enum contains types of render targets.
  enum class RenderTarget {
    kNull,
    kPrint,
    kScreen
  };

  /// \brief Constructor.
  OnSagView();

  /// \brief Destructor.
  ~OnSagView();

  /// \brief Gets the graphics plot rect.
  /// \return The graphics plot rect.
  wxRect GraphicsPlotRect() const;

  /// \brief Handles closing the view.
  /// \param[in] delete_window
  ///   An indicator that determines if the child windows should be deleted.
  /// \return true
  /// This function is called by wxWidgets.
  virtual bool OnClose(bool delete_window = true);

  /// \brief Creates the view and initializes.
  /// \param[in] doc
  ///   The document.
  /// \param[in] flags
  ///   The parameters used by wxWidgets.
  /// \return Always true.
  /// This function is called by wxWidgets.
  virtual bool OnCreate(wxDocument *doc, long flags);

  /// \brief Creates a printout.
  /// \return A printout.
  virtual wxPrintout* OnCreatePrintout();

  /// \brief Handles drawing/rendering the view.
  /// \param[in] dc
  ///   The device context.
  /// This function is called by wxWidgets.
  virtual void OnDraw(wxDC *dc);

  /// \brief Handles the print event.
  /// \param[in] event
  ///   The event.
  void OnPrint(wxCommandEvent& event);

  /// \brief Handles the print preview event.
  /// \param[in] event
  ///   The event.
  void OnPrintPreview(wxCommandEvent& event);

  /// \brief Handles updating of the view.
  /// \param[in] sender
  ///   The view that sent the update request. Since this is a single view
  ///   application, this will remain nullptr.
  /// \param[in] hint
  ///   The update hint that helps child windows optimize the update. This is
  ///   provided by the function/action that triggered the update.
  /// This function is called by wxWidgets.
  virtual void OnUpdate(wxView *sender, wxObject *hint = nullptr);

  /// \brief Gets the result index.
  /// \return The result index.
  const int index_result() const;

  /// \brief Gets the edit pane.
  /// \return The edit pane.
  EditPane* pane_edit();

  /// \brief Gets the profile plot pane.
  /// \return The profile plot pane.
  ProfilePlotPane* pane_plot();

  /// \brief Gets the results pane.
  /// \return The results pane.
  ResultsPane* pane_results();

  /// \brief Sets the selected analysis result.
  /// \param[in] index
  ///   The selected result index.
  void set_index_result(const int& index);

  /// \brief Sets the render target.
  /// \param[in] target_render
  ///   The render target.
  void set_target_render(const RenderTarget& target_render);

  /// \brief Gets the render target.
  /// \return The render target.
  RenderTarget target_render() const;

 private:
  /// \var index_result_
  ///   The result index that is currently selected. If no result is displayed,
  ///   set to -1.
  int index_result_;

  /// \var pane_edit_
  ///   The edit pane.
  EditPane* pane_edit_;

  /// \var pane_plot_
  ///   The plot pane.
  ProfilePlotPane* pane_plot_;

  /// \var pane_results_
  ///   The results pane.
  ResultsPane* pane_results_;

  /// \var target_render_
  ///   The render target.
  RenderTarget target_render_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(OnSagView);

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_ONSAG_ONSAGVIEW_H_

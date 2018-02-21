// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_ONSAGVIEW_H_
#define OTLS_ONSAG_ONSAGVIEW_H_

#include "wx/docview.h"

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

  /// \brief Handles drawing/rendering the view.
  /// \param[in] dc
  ///   The device context.
  /// This function is called by wxWidgets.
  virtual void OnDraw(wxDC *dc);

  /// \brief Handles updating of the view.
  /// \param[in] sender
  ///   The view that sent the update request. Since this is a single view
  ///   application, this will remain nullptr.
  /// \param[in] hint
  ///   The update hint that helps child windows optimize the update. This is
  ///   provided by the function/action that triggered the update.
  /// This function is called by wxWidgets.
  virtual void OnUpdate(wxView *sender, wxObject *hint = nullptr);

  /// \brief Sets the render target.
  /// \param[in] target_render
  ///   The render target.
  void set_target_render(const RenderTarget& target_render);

  /// \brief Gets the render target.
  /// \return The render target.
  RenderTarget target_render() const;

 private:
  /// \var target_render_
  ///   The render target.
  RenderTarget target_render_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(OnSagView);
};

#endif  // OTLS_ONSAG_ONSAGVIEW_H_

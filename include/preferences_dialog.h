// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_PREFERENCESDIALOG_H_
#define OTLS_ONSAG_PREFERENCESDIALOG_H_

#include "wx/wx.h"

#include "on_sag_config.h"

/// \par OVERVIEW
///
/// This class is the application preferences editor dialog.
class PreferencesDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] config
  ///   The application configuration settings.
  PreferencesDialog(wxWindow* parent, OnSagConfig* config);

  /// \brief Destructor.
  ~PreferencesDialog();

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnButtonCancel(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnButtonOk(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

 private:
  /// \var config_
  ///   The application configuration data.
  OnSagConfig* config_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_ONSAG_PREFERENCESDIALOG_H_

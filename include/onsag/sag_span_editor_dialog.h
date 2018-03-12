// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGSPANEDITORDIALOG_H_
#define OTLS_ONSAG_SAGSPANEDITORDIALOG_H_

#include "models/base/units.h"
#include "wx/wx.h"

#include "onsag/sag_span.h"

/// \par OVERVIEW
///
/// This class is a sag span editor dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. Data is
/// transferred between the data objects and controls with validators when
/// possible. Customized methods are used for anything else that remains.
class SagSpanEditorDialog : public wxDialog {
 public:
  /// \brief Default constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] units
  ///   The unit system to display on the form.
  /// \param[in] span
  ///   The span to be modified with the dialog.
  SagSpanEditorDialog(wxWindow* parent,
                      const units::UnitSystem& units,
                      SagSpan* span);

  /// \brief Destructor.
  ~SagSpanEditorDialog();

 private:
  /// \brief Initializes any missing tension points.
  void InitializeMissingTensionPoints();

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Handles the dynamometer method radio button event.
  /// \param[in] event
  ///   The event.
  void OnRadioButtonDynamometer(wxCommandEvent& event);

  /// \brief Handles the stopwatch method radio button event.
  /// \param[in] event
  ///   The event.
  void OnRadioButtonStopwatch(wxCommandEvent& event);

  /// \brief Handles the transit method radio button event.
  /// \param[in] event
  ///   The event.
  void OnRadioButtonTransit(wxCommandEvent& event);

  /// \brief Changes static text to display the desired unit system.
  /// \param[in] units
  ///   The unit system to display.
  void SetUnitsStaticText(const units::UnitSystem& units);

  /// \brief Sets the control validators on the form.
  void SetValidators();

  /// \brief Toggles the controls for the dynamometer sagging mode.
  /// \param[in] enable
  ///   An indicator that tells whether to enable or disable the controls.
  /// If the controls are disabled, but the values are not cleared.
  void ToggleControlsDynamometer(bool enable);

  /// \brief Toggles the controls for the stopwatch sagging mode.
  /// \param[in] enable
  ///   An indicator that tells whether to enable or disable the controls.
  /// If the controls are disabled, but the values are not cleared.
  void ToggleControlsStopwatch(bool enable);

  /// \brief Toggles the controls for the transit sagging mode.
  /// \param[in] enable
  ///   An indicator that tells whether to enable or disable the controls.
  /// If the controls are disabled, but the values are not cleared.
  void ToggleControlsTransit(bool enable);

  /// \brief Transfers data that isn't tied to a validator from the window to
  ///   the span object.
  void TransferCustomDataFromWindow();

  /// \brief Transfers data that isn't tied to a validator from the span object
  ///   to the window.
  void TransferCustomDataToWindow();

  /// \var span_
  ///   The sag span that is edited.
  SagSpan* span_;

  /// \var span_modified_
  ///   The span that is modified and tied to the dialog controls.
  SagSpan span_modified_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_ONSAG_SAGSPANEDITORDIALOG_H_

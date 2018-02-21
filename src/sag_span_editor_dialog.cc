// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "sag_span_editor_dialog.h"

#include "appcommon/widgets/error_message_dialog.h"
#include "models/base/helper.h"
#include "wx/notebook.h"
#include "wx/settings.h"
#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(SagSpanEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, SagSpanEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, SagSpanEditorDialog::OnOk)
  EVT_CLOSE(SagSpanEditorDialog::OnClose)
  EVT_RADIOBUTTON(XRCID("radiobutton_dynamometer"), SagSpanEditorDialog::OnRadioButtonDynamometer)
  EVT_RADIOBUTTON(XRCID("radiobutton_stopwatch"), SagSpanEditorDialog::OnRadioButtonStopwatch)
  EVT_RADIOBUTTON(XRCID("radiobutton_transit"), SagSpanEditorDialog::OnRadioButtonTransit)
END_EVENT_TABLE()

SagSpanEditorDialog::SagSpanEditorDialog(
    wxWindow* parent,
    const units::UnitSystem& units,
    SagSpan* span) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "sag_span_editor_dialog");
  this->Fit();

  // saves constructor parameters to class
  SetUnitsStaticText(units);

  // saves unmodified span reference, and copies to modified span
  span_ = span;
  span_modified_ = SagSpan(*span);

  InitializeMissingTensionPoints();

  // sets natural theme colors for notebook pages
  wxPanel* panel = nullptr;

  panel = XRCCTRL(*this, "panel_cable", wxPanel);
  panel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

  panel = XRCCTRL(*this, "panel_structures", wxPanel);
  panel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

  panel = XRCCTRL(*this, "panel_sagging", wxPanel);
  panel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

  panel = XRCCTRL(*this, "panel_notes", wxPanel);
  panel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

  // sets form validators to transfer between controls data and controls
  SetValidators();

  // inits form controls
  TransferDataToWindow();
  TransferCustomDataToWindow();

  if (span_modified_.method.type == SagMethod::Type::kDynamometer) {
    ToggleControlsDynamometer(true);
    ToggleControlsStopwatch(false);
    ToggleControlsTransit(false);
  } else if (span_modified_.method.type == SagMethod::Type::kStopWatch) {
    ToggleControlsDynamometer(false);
    ToggleControlsStopwatch(true);
    ToggleControlsTransit(false);
  } else if (span_modified_.method.type == SagMethod::Type::kTransit) {
    ToggleControlsDynamometer(false);
    ToggleControlsStopwatch(false);
    ToggleControlsTransit(true);
  }
}

SagSpanEditorDialog::~SagSpanEditorDialog() {
}

void SagSpanEditorDialog::InitializeMissingTensionPoints() {
  // need at least 5 tension points
  while (span_modified_.cable.tensions.size() < 5) {
    SagCable::TensionPoint point;
    point.temperature = -999999;
    point.tension_horizontal = -999999;
    span_modified_.cable.tensions.push_back(point);
  }
}

void SagSpanEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void SagSpanEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void SagSpanEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  wxBusyCursor cursor;

  // transfers data from dialog controls
  TransferDataFromWindow();
  TransferCustomDataFromWindow();

  // validates span data
  std::list<ErrorMessage> messages;
  if (span_modified_.Validate(true, &messages) == true) {

    // updates original cable reference based on user form edits
    *span_ = SagSpan(span_modified_);

    // ends modal by returning ok indicator
    EndModal(wxID_OK);
  } else {
    // displays errors to user
    ErrorMessageDialog message(this, &messages);
    message.ShowModal();
    return;
  }
}

void SagSpanEditorDialog::OnRadioButtonDynamometer(wxCommandEvent& event) {
  ToggleControlsDynamometer(true);
  ToggleControlsStopwatch(false);
  ToggleControlsTransit(false);
}

void SagSpanEditorDialog::OnRadioButtonStopwatch(wxCommandEvent& event) {
  ToggleControlsDynamometer(false);
  ToggleControlsStopwatch(true);
  ToggleControlsTransit(false);
}

void SagSpanEditorDialog::OnRadioButtonTransit(wxCommandEvent& event) {
  ToggleControlsDynamometer(false);
  ToggleControlsStopwatch(false);
  ToggleControlsTransit(true);
}

void SagSpanEditorDialog::SetUnitsStaticText(const units::UnitSystem& units) {
  if (units == units::UnitSystem::kMetric) {
    // cable notebook page
    XRCCTRL(*this, "statictext_cable_weight_units",
            wxStaticText)->SetLabel("[N/m]");
    XRCCTRL(*this, "statictext_cable_temperature_units",
            wxStaticText)->SetLabel("[degC]");
    XRCCTRL(*this, "statictext_cable_tension_units",
            wxStaticText)->SetLabel("[N]");
    XRCCTRL(*this, "statictext_correction_creep_units",
            wxStaticText)->SetLabel("[degC]");
    XRCCTRL(*this, "statictext_correction_sag_units",
            wxStaticText)->SetLabel("[cm]");
    XRCCTRL(*this, "statictext_scale_units",
            wxStaticText)->SetLabel("[ ]");

    // structures notebook page
    XRCCTRL(*this, "statictext_back_x_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_back_y_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_ahead_x_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_ahead_y_units",
            wxStaticText)->SetLabel("[m]");

    // sagging notebook page
    XRCCTRL(*this, "statictext_transit_x_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_transit_y_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_temperature_base_units",
            wxStaticText)->SetLabel("[degC]");
    XRCCTRL(*this, "statictext_temperature_interval_units",
            wxStaticText)->SetLabel("[degC]");
  } else if (units == units::UnitSystem::kImperial) {
    // cable notebook page
    XRCCTRL(*this, "statictext_cable_weight_units",
            wxStaticText)->SetLabel("[lb/ft]");
    XRCCTRL(*this, "statictext_cable_temperature_units",
            wxStaticText)->SetLabel("[degF]");
    XRCCTRL(*this, "statictext_cable_tension_units",
            wxStaticText)->SetLabel("[lbs]");
    XRCCTRL(*this, "statictext_correction_creep_units",
            wxStaticText)->SetLabel("[degF]");
    XRCCTRL(*this, "statictext_correction_sag_units",
            wxStaticText)->SetLabel("[in]");
    XRCCTRL(*this, "statictext_scale_units",
            wxStaticText)->SetLabel("[ ]");

    // structures notebook page
    XRCCTRL(*this, "statictext_back_x_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_back_y_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_ahead_x_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_ahead_y_units",
            wxStaticText)->SetLabel("[ft]");

    // sagging notebook page
    XRCCTRL(*this, "statictext_transit_x_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_transit_y_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_temperature_base_units",
            wxStaticText)->SetLabel("[degF]");
    XRCCTRL(*this, "statictext_temperature_interval_units",
            wxStaticText)->SetLabel("[degF]");
  }
}

void SagSpanEditorDialog::SetValidators() {
  // variables used for creating validators
  int style = 0;
  double* value = nullptr;
  int precision = 0;
  wxString str;
  wxTextCtrl* textctrl = nullptr;

  // cable name
  style = wxFILTER_NONE;
  textctrl = XRCCTRL(*this, "textctrl_cable_name", wxTextCtrl);
  textctrl->SetValidator(wxTextValidator(style, nullptr));

  // cable weight-unit
  precision = 3;
  value = &span_modified_.cable.weight_unit;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_cable_weight", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #1 temperature
  precision = 1;
  value = &(*std::next(span_modified_.cable.tensions.begin(), 0)).temperature;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_1", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #1 tension
  precision = 1;
  value =
    &(*std::next(span_modified_.cable.tensions.begin(), 0)).tension_horizontal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_tension_1", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #2 temperature
  precision = 1;
  value = &(*std::next(span_modified_.cable.tensions.begin(), 1)).temperature;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_2", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #2 tension
  precision = 1;
  value =
    &(*std::next(span_modified_.cable.tensions.begin(), 1)).tension_horizontal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_tension_2", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #3 temperature
  precision = 1;
  value = &(*std::next(span_modified_.cable.tensions.begin(), 2)).temperature;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_3", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #3 tension
  precision = 1;
  value =
    &(*std::next(span_modified_.cable.tensions.begin(), 2)).tension_horizontal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_tension_3", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #4 temperature
  precision = 1;
  value = &(*std::next(span_modified_.cable.tensions.begin(), 3)).temperature;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_4", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #4 tension
  precision = 1;
  value =
    &(*std::next(span_modified_.cable.tensions.begin(), 3)).tension_horizontal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_tension_4", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #5 temperature
  precision = 1;
  value = &(*std::next(span_modified_.cable.tensions.begin(), 4)).temperature;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_5", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable - point #5 tension
  precision = 1;
  value =
    &(*std::next(span_modified_.cable.tensions.begin(), 4)).tension_horizontal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_tension_5", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable correction-creep
  precision = 1;
  value = &span_modified_.cable.correction_creep;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_correction_creep", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable correction-sag
  precision = 1;
  value = &span_modified_.cable.correction_sag;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_correction_sag", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // cable scaling factor
  precision = 2;
  value = &span_modified_.cable.scale;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_scale", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // structure back-x
  precision = 2;
  value = &span_modified_.structure_back.point_attachment.x;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_back_x", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // structure back-y
  precision = 2;
  value = &span_modified_.structure_back.point_attachment.y;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_back_y", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // structure ahead-x
  precision = 2;
  value = &span_modified_.structure_ahead.point_attachment.x;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_ahead_x", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // structure ahead-y
  precision = 2;
  value = &span_modified_.structure_ahead.point_attachment.y;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_ahead_y", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // transit-x
  precision = 2;
  value = &span_modified_.method.point_transit.x;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_transit_x", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // transit-y
  precision = 2;
  value = &span_modified_.method.point_transit.y;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_transit_y", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // wave-return
  int* value_int = nullptr;
  value_int = &span_modified_.method.wave_return;
  style = wxNUM_VAL_DEFAULT;
  textctrl = XRCCTRL(*this, "textctrl_wave_return", wxTextCtrl);
  textctrl->SetValidator(
      wxIntegerValidator<int>(value_int, style));

  // temperature-base
  precision = 1;
  value = &span_modified_.temperature_base;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_base", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));

  // temperature-interval
  precision = 1;
  value = &span_modified_.temperature_interval;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_temperature_interval", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value, style));
}

void SagSpanEditorDialog::ToggleControlsDynamometer(bool enable) {
  wxRadioButton* button = nullptr;

  if (enable == true) {
    button = XRCCTRL(*this, "radiobutton_structure_back", wxRadioButton);
    button->Enable();

    button = XRCCTRL(*this, "radiobutton_structure_ahead", wxRadioButton);
    button->Enable();
  } else {
    button = XRCCTRL(*this, "radiobutton_structure_back", wxRadioButton);
    button->Disable();

    button = XRCCTRL(*this, "radiobutton_structure_ahead", wxRadioButton);
    button->Disable();
  }
}

void SagSpanEditorDialog::ToggleControlsStopwatch(bool enable) {
  wxStaticText* statictext = nullptr;
  wxTextCtrl* textctrl = nullptr;

  if (enable == true) {
    statictext = XRCCTRL(*this, "statictext_wave_return", wxStaticText);
    statictext->Enable();

    textctrl = XRCCTRL(*this, "textctrl_wave_return", wxTextCtrl);
    textctrl->Enable();
  } else {
    statictext = XRCCTRL(*this, "statictext_wave_return", wxStaticText);
    statictext->Disable();

    textctrl = XRCCTRL(*this, "textctrl_wave_return", wxTextCtrl);
    textctrl->Disable();
  }
}

void SagSpanEditorDialog::ToggleControlsTransit(bool enable) {
  wxStaticText* statictext = nullptr;
  wxTextCtrl* textctrl = nullptr;

  if (enable == true) {
    statictext = XRCCTRL(*this, "statictext_transit_x", wxStaticText);
    statictext->Enable();

    statictext = XRCCTRL(*this, "statictext_transit_x_units", wxStaticText);
    statictext->Enable();

    textctrl = XRCCTRL(*this, "textctrl_transit_x", wxTextCtrl);
    textctrl->Enable();

    statictext = XRCCTRL(*this, "statictext_transit_y", wxStaticText);
    statictext->Enable();

    statictext = XRCCTRL(*this, "statictext_transit_y_units", wxStaticText);
    statictext->Enable();

    textctrl = XRCCTRL(*this, "textctrl_transit_y", wxTextCtrl);
    textctrl->Enable();
  } else {
    statictext = XRCCTRL(*this, "statictext_transit_x", wxStaticText);
    statictext->Disable();

    statictext = XRCCTRL(*this, "statictext_transit_x_units", wxStaticText);
    statictext->Disable();

    textctrl = XRCCTRL(*this, "textctrl_transit_x", wxTextCtrl);
    textctrl->Disable();

    statictext = XRCCTRL(*this, "statictext_transit_y", wxStaticText);
    statictext->Disable();

    statictext = XRCCTRL(*this, "statictext_transit_y_units", wxStaticText);
    statictext->Disable();

    textctrl = XRCCTRL(*this, "textctrl_transit_y", wxTextCtrl);
    textctrl->Disable();
  }
}

void SagSpanEditorDialog::TransferCustomDataFromWindow() {
  wxTextCtrl* textctrl = nullptr;
  wxString str;

  // transfers description
  textctrl = XRCCTRL(*this, "textctrl_description", wxTextCtrl);
  span_modified_.description = textctrl->GetValue();

  // transfers cable-name
  textctrl = XRCCTRL(*this, "textctrl_cable_name", wxTextCtrl);
  span_modified_.cable.name = textctrl->GetValue();

  // transfers back-name
  textctrl = XRCCTRL(*this, "textctrl_back_structure", wxTextCtrl);
  span_modified_.structure_back.name = textctrl->GetValue();

  // transfers back-attachment
  textctrl = XRCCTRL(*this, "textctrl_back_attachment", wxTextCtrl);
  span_modified_.structure_back.attachment = textctrl->GetValue();

  // transfers ahead-name
  textctrl = XRCCTRL(*this, "textctrl_ahead_structure", wxTextCtrl);
  span_modified_.structure_ahead.name = textctrl->GetValue();

  // transfers ahead-attachment
  textctrl = XRCCTRL(*this, "textctrl_ahead_attachment", wxTextCtrl);
  span_modified_.structure_ahead.attachment = textctrl->GetValue();

  // transfers method-end
  if (XRCCTRL(*this, "radiobutton_structure_ahead", wxRadioButton)
      ->GetValue() == true) {
    span_modified_.method.end = SagMethod::SpanEndType::kAheadOnLine;
  } else if (XRCCTRL(*this, "radiobutton_structure_back", wxRadioButton)
      ->GetValue() == true) {
    span_modified_.method.end = SagMethod::SpanEndType::kBackOnLine;
  }

  // transfers method-type
  if (XRCCTRL(*this, "radiobutton_dynamometer", wxRadioButton)
      ->GetValue() == true) {
    span_modified_.method.type = SagMethod::Type::kDynamometer;
  } else if (XRCCTRL(*this, "radiobutton_stopwatch", wxRadioButton)
      ->GetValue() == true) {
    span_modified_.method.type = SagMethod::Type::kStopWatch;
  } else if (XRCCTRL(*this, "radiobutton_transit", wxRadioButton)
      ->GetValue() == true) {
    span_modified_.method.type = SagMethod::Type::kTransit;
  }
}

void SagSpanEditorDialog::TransferCustomDataToWindow() {
  wxRadioButton* button_radio = nullptr;
  wxTextCtrl* textctrl = nullptr;
  wxString str;

  // transfers description
  textctrl = XRCCTRL(*this, "textctrl_description", wxTextCtrl);
  textctrl->SetValue(span_modified_.description);

  // transfers cable-name
  textctrl = XRCCTRL(*this, "textctrl_cable_name", wxTextCtrl);
  textctrl->SetValue(span_modified_.cable.name);

  // transfers back-name
  textctrl = XRCCTRL(*this, "textctrl_back_structure", wxTextCtrl);
  textctrl->SetValue(span_modified_.structure_back.name);

  // transfers back-attachment
  textctrl = XRCCTRL(*this, "textctrl_back_attachment", wxTextCtrl);
  textctrl->SetValue(span_modified_.structure_back.attachment);

  // transfers ahead-name
  textctrl = XRCCTRL(*this, "textctrl_ahead_structure", wxTextCtrl);
  textctrl->SetValue(span_modified_.structure_ahead.name);

  // transfers ahead-attachment
  textctrl = XRCCTRL(*this, "textctrl_ahead_attachment", wxTextCtrl);
  textctrl->SetValue(span_modified_.structure_ahead.attachment);

  // transfers method-end
  if (span_modified_.method.end == SagMethod::SpanEndType::kAheadOnLine) {
    button_radio = XRCCTRL(*this, "radiobutton_structure_ahead", wxRadioButton);
    button_radio->SetValue(true);
  } else if (span_modified_.method.end == SagMethod::SpanEndType::kBackOnLine) {
    button_radio = XRCCTRL(*this, "radiobutton_structure_back", wxRadioButton);
    button_radio->SetValue(true);
  }

  // transfers method-type
  if (span_modified_.method.type == SagMethod::Type::kDynamometer) {
    button_radio = XRCCTRL(*this, "radiobutton_dynamometer", wxRadioButton);
    button_radio->SetValue(true);
  } else if (span_modified_.method.type == SagMethod::Type::kStopWatch) {
    button_radio = XRCCTRL(*this, "radiobutton_stopwatch", wxRadioButton);
    button_radio->SetValue(true);
  } else if (span_modified_.method.type == SagMethod::Type::kTransit) {
    button_radio = XRCCTRL(*this, "radiobutton_transit", wxRadioButton);
    button_radio->SetValue(true);
  }
}

// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/preferences_dialog.h"

#include "wx/clrpicker.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, PreferencesDialog::OnButtonCancel)
  EVT_BUTTON(wxID_OK, PreferencesDialog::OnButtonOk)
  EVT_CLOSE(PreferencesDialog::OnClose)
END_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(
    wxWindow* parent,
    OnSagConfig* config) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "preferences_dialog");

  // saves doc reference
  config_ = config;

  // sets the unit system in the radio control
  wxRadioBox* radiobox = nullptr;

  radiobox = XRCCTRL(*this, "radiobox_units", wxRadioBox);
  if (config_->units == units::UnitSystem::kImperial) {
    radiobox->SetSelection(0);
  } else if (config_->units == units::UnitSystem::kMetric) {
    radiobox->SetSelection(1);
  }

  // sets the logging level in the radio control
  radiobox = XRCCTRL(*this, "radiobox_logging", wxRadioBox);
  if (config_->level_log == wxLOG_Message) {
    radiobox->SetSelection(0);
  } else if (config_->level_log == wxLOG_Info) {
    radiobox->SetSelection(1);
  }

  // sets the color in the color picker
  wxColourPickerCtrl* pickerctrl =
      XRCCTRL(*this, "colorpicker_background", wxColourPickerCtrl);
  pickerctrl->SetColour(config_->color_background);

  // fits the dialog around the sizers
  this->Fit();
}

PreferencesDialog::~PreferencesDialog() {
}

void PreferencesDialog::OnButtonCancel(wxCommandEvent& event) {
  EndModal(wxID_CANCEL);
}

void PreferencesDialog::OnButtonOk(wxCommandEvent& event) {
  wxBusyCursor cursor;

  wxRadioBox* radiobox = nullptr;

  // transfers units
  radiobox = XRCCTRL(*this, "radiobox_units", wxRadioBox);
  if (radiobox->GetSelection() == 0) {
    config_->units = units::UnitSystem::kImperial;
  } else if (radiobox->GetSelection() == 1) {
    config_->units = units::UnitSystem::kMetric;
  }

  // transfers logging level
  radiobox = XRCCTRL(*this, "radiobox_logging", wxRadioBox);
  if (radiobox->GetSelection() == 0) {
    config_->level_log = wxLOG_Message;
  } else if (radiobox->GetSelection() == 1) {
    config_->level_log = wxLOG_Info;
  }

  // transfers background color
  wxColourPickerCtrl* pickerctrl =
      XRCCTRL(*this, "colorpicker_background", wxColourPickerCtrl);
  config_->color_background = pickerctrl->GetColour();

  EndModal(wxID_OK);
}

void PreferencesDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}

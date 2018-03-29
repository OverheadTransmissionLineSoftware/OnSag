// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/on_sag_frame.h"

#include "wx/aboutdlg.h"
#include "wx/printdlg.h"
#include "wx/splitter.h"
#include "wx/xrc/xmlres.h"

#include "onsag/file_handler.h"
#include "onsag/on_sag_app.h"
#include "onsag/on_sag_doc.h"
#include "onsag/on_sag_view.h"
#include "onsag/preferences_dialog.h"
#include "xpm/icon.xpm"

DocumentFileDropTarget::DocumentFileDropTarget(wxWindow* parent) {
  parent_ = parent;

  // creates data object to store dropped information
  SetDataObject(new wxFileDataObject());
  SetDefaultAction(wxDragCopy);
}

bool DocumentFileDropTarget::OnDropFiles(wxCoord x, wxCoord y,
                                         const wxArrayString& filenames) {
  // gets data from drag-and-drop operation
  wxFileDataObject* data = dynamic_cast<wxFileDataObject*>(GetDataObject());

  // tests if file exists
  // only the first file in the list is processed
  const wxString& str_file = data->GetFilenames().front();
  if (wxFileName::Exists(str_file) == false) {
    return false;
  }

  // tests if extension matches application document
  wxFileName path(str_file);
  if (path.GetExt() == ".onsag") {
    return false;
  }

  // freezes frame, opens document, and thaws frame
  parent_->Freeze();
  wxGetApp().manager_doc()->CreateDocument(str_file);
  parent_->Thaw();

  return true;
}

BEGIN_EVENT_TABLE(OnSagFrame, wxDocParentFrame)
  EVT_MENU(XRCID("menuitem_file_pagesetup"), OnSagFrame::OnMenuFilePageSetup)
  EVT_MENU(XRCID("menuitem_file_preferences"), OnSagFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_help_about"), OnSagFrame::OnMenuHelpAbout)
  EVT_MENU(XRCID("menuitem_help_manual"), OnSagFrame::OnMenuHelpManual)
  EVT_MENU(XRCID("menuitem_view_log"), OnSagFrame::OnMenuViewLog)
  EVT_SIZE(OnSagFrame::OnResize)
END_EVENT_TABLE()

OnSagFrame::OnSagFrame(wxDocManager* manager)
    : wxDocParentFrame(manager, nullptr, wxID_ANY,
                       wxGetApp().GetAppDisplayName()) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadMenuBar(this, "on_sag_menubar");

  // sets the drag and drop target
  SetDropTarget(new DocumentFileDropTarget(this));

  // sets the frame icon
  SetIcon(wxIcon(icon_xpm));

  // creates status bar
  wxStatusBar* status_bar = CreateStatusBar();
  const int kFieldsCount = 2;
  const int widths_field[2] = {-1, 170};
  status_bar->SetFieldsCount(kFieldsCount);
  status_bar->SetStatusWidths(kFieldsCount, widths_field);

  // tells aui manager to manage this frame
  manager_.SetManagedWindow(this);

  // creates log AUI window and adds to manager
  wxAuiPaneInfo info;
  info.Name("Log");
  info.Float();
  info.Caption("Log");
  info.CloseButton(true);
  info.Show(false);
  pane_log_ = new LogPane(this);
  manager_.AddPane(pane_log_, info);

  manager_.Update();
}

OnSagFrame::~OnSagFrame() {
  // saves if frame is maximized to application config
  OnSagConfig* config = wxGetApp().config();
  config->is_maximized_frame = IsMaximized();

  // resets aui manager
  manager_.UnInit();
}

void OnSagFrame::OnMenuFilePageSetup(wxCommandEvent& event) {
  // gets application page setup data
  wxPageSetupDialogData* data_page = wxGetApp().config()->data_page;

  // creates and shows dialog
  wxPageSetupDialog dialog(this, data_page);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // updates print data
  *data_page = dialog.GetPageSetupDialogData();
}

void OnSagFrame::OnMenuFilePreferences(wxCommandEvent& event) {
  // gets the application config
  OnSagConfig* config = wxGetApp().config();

  // stores a copy of the unit system before letting user edit
  units::UnitSystem units_before = config->units;

  // creates preferences editor dialog and shows
  // exits if user closes/cancels
  PreferencesDialog preferences(this, config);
  if (preferences.ShowModal() != wxID_OK) {
    return;
  }

  wxBusyCursor cursor;

  // application data change is implemented on app restart

  // updates logging level
  wxLog::SetLogLevel(config->level_log);
  if (config->level_log == wxLOG_Message) {
    wxLog::SetVerbose(false);
  } else if (config->level_log == wxLOG_Info) {
    wxLog::SetVerbose(true);
  }

  // converts unit system if it changed
  OnSagDoc* doc = wxGetApp().GetDocument();
  if (units_before != config->units) {
    wxLogVerbose("Converting unit system.");

    // updates document
    if (doc != nullptr) {
      doc->ConvertUnitSystem(units_before, config->units);
      doc->RunAnalysis();
    }
  }

  // updates views
  UpdateHint hint(UpdateHint::Type::kPreferencesEdit);
  doc->UpdateAllViews(nullptr, &hint);
}

void OnSagFrame::OnMenuHelpAbout(wxCommandEvent& event) {
  // sets the dialog info
  wxAboutDialogInfo info;
  info.SetIcon(wxIcon(icon_xpm));
  info.SetName(wxGetApp().GetAppDisplayName());
  info.SetVersion(wxGetApp().version());
  info.SetCopyright("License:   http://unlicense.org/");
  info.SetDescription(
    "This is an open-source application that models transmission cables for "
    "sagging.\n"
    "\n"
    "To get involved in the project development or to review the code, see the "
    "website.");
  info.SetWebSite("https://github.com/OverheadTransmissionLineSoftware/OnSag");

  // shows the dialog
  wxAboutBox(info, this);
}

void OnSagFrame::OnMenuHelpManual(wxCommandEvent& event) {
  wxHtmlHelpController* help = wxGetApp().help();
  help->DisplayContents();
}

void OnSagFrame::OnMenuViewLog(wxCommandEvent& event) {
  // toggles the log window visibility
  wxAuiPaneInfo& info = manager_.GetPane("Log");
  if (info.IsShown() == false) {
    info.Show(true);
  } else {
    info.Show(false);
  }

  manager_.Update();
}

void OnSagFrame::OnResize(wxSizeEvent& event) {
  // skips caching frame size if maximized
  if (IsMaximized() == true) {
    return;
  }

  // saves frame size to application config
  OnSagConfig* config = wxGetApp().config();
  config->size_frame = event.GetSize();
}

LogPane* OnSagFrame::pane_log() {
  return pane_log_;
}

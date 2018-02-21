// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "on_sag_app.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/xrc/xmlres.h"

#include "file_handler.h"
#include "on_sag_doc.h"
#include "on_sag_view.h"

/// This function handles loading all of the xrc files that are processed by
/// the wxWidgets resource compiler. It is declared in an external file so
/// this file doesn't have to be recompiled every time the resources are
/// updated.
extern void InitXmlResource();

IMPLEMENT_APP(OnSagApp)

OnSagApp::OnSagApp() {
  frame_ = nullptr;
  manager_doc_ = nullptr;
}

OnSagApp::~OnSagApp() {
}

OnSagDoc* OnSagApp::GetDocument() const {
  return dynamic_cast<OnSagDoc*>(manager_doc_->GetCurrentDocument());
}

bool OnSagApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  // gets the config file path
  wxString filepath_config;
  if (parser.Found("config", &filepath_config)) {
    // converts filepath to absolute if needed
    wxFileName path(filepath_config);
    if (path.IsAbsolute() == false) {
      path.MakeAbsolute(wxEmptyString, wxPATH_NATIVE);
    }

    filepath_config_ = path.GetFullPath();
  } else {
    wxFileName path(wxEmptyString, "onsag", "conf");

    // detects OS and specifies default config file path for user
    wxOperatingSystemId os = wxGetOsVersion();
    if ((wxOS_UNKNOWN < os) && (os <= wxOS_MAC)) {
      // mac os
      path.SetPath(wxStandardPaths::Get().GetExecutablePath());
    } else if ((wxOS_MAC < os) && (os <= wxOS_WINDOWS)) {
      // windows os
      path.SetPath(wxStandardPaths::Get().GetUserConfigDir());
      path.AppendDir("OTLS");
      path.AppendDir("OnSag");
    } else if ((wxOS_WINDOWS < os) && (os <= wxOS_UNIX)) {
      // unix os
      path.SetPath(wxStandardPaths::Get().GetUserConfigDir());
      path.AppendDir(".config");
      path.AppendDir("otls");
      path.AppendDir("on-sag");
    } else {
      path.SetPath(wxStandardPaths::Get().GetExecutablePath());
    }

    filepath_config_ = path.GetFullPath();
  }

  // captures the start file which will be loaded when doc manager is created
  if (parser.GetParamCount() == 1) {
    filepath_start_ = parser.GetParam(0);
  }

  return true;
}

int OnSagApp::OnExit() {
  // disables logging because all windows are destroyed
  wxLog::EnableLogging(false);

  // saves config file
  FileHandler::SaveConfigFile(filepath_config_, config_);

  // cleans up allocated resources
  delete manager_doc_;

  // continues exit process
  return wxApp::OnExit();
}

bool OnSagApp::OnInit() {
  wxFileName filename;

  // initializes command line
  if (wxApp::OnInit() == false) {
    return false;
  }

  // sets application info
  SetAppDisplayName("OnSag");
  SetAppName("OnSag");
  version_ = "0.0.0";

  // creates a document manager and sets to single document interface
  manager_doc_ = new wxDocManager();
  manager_doc_->SetMaxDocsOpen(1);

  // creates a document/view template relating drawing documents to their views
  (void) new wxDocTemplate(manager_doc_, "OnSag", "*.onsag",
                           "", "onsag", "OnSagDoc", "OnSagView",
                           CLASSINFO(OnSagDoc), CLASSINFO(OnSagView));

  // loads all xml resource files into virtual file system
  wxXmlResource::Get()->InitAllHandlers();
  InitXmlResource();

  // creates main application frame
  frame_ = new OnSagFrame(manager_doc_);
  SetTopWindow(frame_);

  // sets application logging to a modeless dialog managed by the frame
  wxLogTextCtrl* log = new wxLogTextCtrl(frame_->pane_log()->textctrl());
  wxLog::SetActiveTarget(log);

  // manually initailizes application config defaults
  filename = wxFileName(filepath_config_);
  wxPrintData data_print;
  data_print.SetOrientation(wxLANDSCAPE);
  config_.color_background = *wxBLACK;
  config_.is_maximized_frame = true;
  config_.level_log = wxLOG_Message;
  config_.size_frame = wxSize(400, 400);
  config_.units = units::UnitSystem::kImperial;

  // loads config settings from file, or saves a file if it doesn't exist
  // on loading, any settings defined in the file will override the app defaults
  // filehandler handles all logging
  filename = wxFileName(filepath_config_);
  if (filename.Exists() == true) {
    FileHandler::LoadConfigFile(filepath_config_, config_);
  } else {
    FileHandler::SaveConfigFile(filepath_config_, config_);
  }

  // sets log level specified in app config
  wxLog::SetLogLevel(config_.level_log);
  if (config_.level_log == wxLOG_Info) {
    wxLog::SetVerbose(true);
  }

  // sets application frame based on config setting
  // this needs to be done before any messages are shown
  frame_->SetSize(config_.size_frame);
  if (config_.is_maximized_frame == true) {
    frame_->Maximize();
  }

  // loads a document if defined in command line
  if (filepath_start_ != wxEmptyString) {
    manager_doc_->CreateDocument(filepath_start_);
  }

  // shows application frame
  frame_->Centre(wxBOTH);
  frame_->Show(true);

  // updates status bar
  status_bar_log::SetText("Ready", 0);

  return true;
}

void OnSagApp::OnInitCmdLine(wxCmdLineParser& parser) {
  // sets command line style and parameters
  parser.EnableLongOptions();
  parser.SetDesc(cmd_line_desc);
  parser.SetSwitchChars("--");
}

OnSagConfig* OnSagApp::config() {
  return &config_;
}

OnSagFrame* OnSagApp::frame() {
  return frame_;
}

wxDocManager* OnSagApp::manager_doc() {
  return manager_doc_;
}

std::string OnSagApp::version() const {
  return version_;
}

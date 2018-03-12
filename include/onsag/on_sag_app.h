// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_ONSAGAPP_H_
#define OTLS_ONSAG_ONSAGAPP_H_

#include <string>

#include "wx/cmdline.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "onsag/on_sag_config.h"
#include "onsag/on_sag_doc.h"
#include "onsag/on_sag_frame.h"

/// \par OVERVIEW
///
/// This is the OnSag application class.
///
/// \par CONFIGURATION SETTINGS
///
/// The application stores configuration settings. These are loaded on startup
/// and reflect any user specific customizations.
///
/// \par DOCUMENT / VIEW FRAMEWORK
///
/// This class implements the wxWidgets Document/View framework, which allows
/// loading/saving/printing of application files.
class OnSagApp : public wxApp {
 public:
  /// \brief Constructor.
  OnSagApp();

  /// \brief Destructor.
  ~OnSagApp();

  /// \brief Gets the current document.
  /// \return The document. If no document is open, a nullptr is returned.
  OnSagDoc* GetDocument() const;

  /// \brief Parses the command line options provided to the application on
  ///   startup.
  /// \param[in] parser
  ///   The command line parser, which is provided by wxWidgets.
  /// \return The success of command line parsing.
  virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

  /// \brief Cleans up application resources.
  /// \return The wxApp::OnExit return value.
  virtual int OnExit();

  /// \brief Initializes the application.
  /// \return The success of the application initialization.
  virtual bool OnInit();

  /// \brief Initializes the command line parser settings.
  /// \param[in] parser
  ///   The command line parser, which is provided by wxWidgets.
  virtual void OnInitCmdLine(wxCmdLineParser& parser);

  /// \brief Gets the application configuration settings.
  /// \return The application configuration settings.
  OnSagConfig* config();

  /// \brief Gets the main application frame.
  /// \return The main application frame.
  OnSagFrame* frame();

  /// \brief Gets the document manager.
  /// \return The document manager.
  wxDocManager* manager_doc();

  /// \brief Gets the application version.
  /// \return The application version.
  std::string version() const;

 private:
  /// \var config_
  ///   The application configuration settings.
  OnSagConfig config_;

  /// \var filepath_config_
  ///   The config file that is loaded on application startup. This is specified
  ///   as a command line option.
  std::string filepath_config_;

  /// \var filepath_start_
  ///   The file that is loaded on application startup. This is specified as
  ///   a command line option.
  std::string filepath_start_;

  /// \var frame_
  ///   The main application frame.
  OnSagFrame* frame_;

  /// \var manager_doc_
  ///   The document manager.
  wxDocManager* manager_doc_;

  /// \var version_
  ///   The app version.
  std::string version_;
};

/// This is an array of command line options.
static const wxCmdLineEntryDesc cmd_line_desc [] = {
  {wxCMD_LINE_SWITCH, nullptr, "help", "shows this help message",
      wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
  {wxCMD_LINE_OPTION, nullptr, "config", "the application configuration file",
      wxCMD_LINE_VAL_STRING},
  {wxCMD_LINE_PARAM, nullptr, nullptr, "file",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

  {wxCMD_LINE_NONE}
};

DECLARE_APP(OnSagApp)

#endif  // OTLS_ONSAG_ONSAGAPP_H_

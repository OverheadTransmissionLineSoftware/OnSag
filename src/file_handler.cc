// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/file_handler.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "onsag/on_sag_app.h"
#include "onsag/on_sag_config_xml_handler.h"

int FileHandler::LoadConfigFile(const std::string& filepath,
                                OnSagConfig& config) {
  std::string message = "Loading config file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Config file (" + filepath + ") does not exist. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load config file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Config file contains an invalid xml structure. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "on_sag_config") {
    message = filepath + "  --  "
              "Config file contains an invalid xml root. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the XML node and loads into the config struct
  const bool status_node = OnSagConfigXmlHandler::ParseNode(
      root, filepath, config);

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

void FileHandler::SaveConfigFile(const std::string& filepath,
                                 const OnSagConfig& config) {
  // logs
  std::string message = "Saving config file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // generates an xml node
  wxXmlNode* root = OnSagConfigXmlHandler::CreateNode(config);

  // creates any directories that are needed
  wxFileName filename(filepath);
  if (filename.DirExists() == false) {
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  bool status = doc.Save(filepath, 2);
  if (status == false) {
    wxLogError("File didn't save");
  }

  // resets statusbar
  status_bar_log::PopText(0);
}

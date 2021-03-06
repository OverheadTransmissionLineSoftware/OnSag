// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/on_sag_config_xml_handler.h"

#include "appcommon/xml/color_xml_handler.h"
#include "wx/filename.h"

#include "onsag/on_sag_app.h"

wxXmlNode* OnSagConfigXmlHandler::CreateNode(const OnSagConfig& config) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  wxXmlNode* sub_node = nullptr;
  std::string title;
  std::string content;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "on_sag_config");
  node_root->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates log level node
  title = "level_log";
  if (config.level_log == wxLOG_Message) {
    content = "Normal";
  } else if (config.level_log == wxLOG_Info) {
    content = "Verbose";
  } else {
    content = "";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates color-background node
  node_element = ColorXmlHandler::CreateNode(config.color_background,
                                             "background");
  node_root->AddChild(node_element);

  // creates options-plot-profile node
  title = "options_plot_profile";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  sub_node = ColorXmlHandler::CreateNode(
      config.options_plot_profile.color_catenary, "catenary");
  node_element->AddChild(sub_node);

  sub_node = ColorXmlHandler::CreateNode(
      config.options_plot_profile.color_method, "method");
  node_element->AddChild(sub_node);

  title = "scale_horizontal";
  content = std::to_string(config.options_plot_profile.scale_horizontal);
  sub_node = CreateElementNodeWithContent(title, content);
  node_element->AddChild(sub_node);

  title = "scale_vertical";
  content = std::to_string(config.options_plot_profile.scale_vertical);
  sub_node = CreateElementNodeWithContent(title, content);
  node_element->AddChild(sub_node);

  title = "thickness_line";
  content = std::to_string(config.options_plot_profile.thickness_line);
  sub_node = CreateElementNodeWithContent(title, content);
  node_element->AddChild(sub_node);

  node_root->AddChild(node_element);

  // creates perspective node
  title = "perspective";
  content = config.perspective;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates size-frame node
  title = "size_frame";
  content = "";
  node_element = CreateElementNodeWithContent(title, content);

  wxString str;
  str = std::to_string(config.size_frame.GetWidth());
  node_element->AddAttribute("x", str);
  str = std::to_string(config.size_frame.GetHeight());
  node_element->AddAttribute("y", str);
  str = std::to_string(config.is_maximized_frame);
  node_element->AddAttribute("is_maximized", str);
  node_root->AddChild(node_element);

  // creates units node
  title = "units";
  if (config.units == units::UnitSystem::kMetric) {
    content = wxString("Metric");
  } else if (config.units == units::UnitSystem::kImperial) {
    content = wxString("Imperial");
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool OnSagConfigXmlHandler::ParseNode(const wxXmlNode* root,
                                      const std::string& filepath,
                                      OnSagConfig& config) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "on_sag_config") {
    message = FileAndLineNumber(filepath, root) +
              " Invalid root node. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    message = FileAndLineNumber(filepath, root) +
              " Version attribute is missing. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, config);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool OnSagConfigXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                        const std::string& filepath,
                                        OnSagConfig& config) {
  bool status = true;
  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if ((title == "color") && (node->GetAttribute("name") == "background")) {
      status = ColorXmlHandler::ParseNode(node, filepath,
                                          config.color_background);
    } else if (title == "level_log") {
      if (content == "Normal") {
        config.level_log = wxLOG_Message;
      } else if (content == "Verbose") {
        config.level_log = wxLOG_Info;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Logging level isn't recognized. Keeping default "
                  "setting.";
        wxLogWarning(message);
      }
    } else if (title == "options_plot_profile") {
      // gets sub-nodes
      wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        wxString sub_title = sub_node->GetName();
        wxString sub_content = ParseElementNodeWithContent(sub_node);
        long value = -9999;

        if (sub_title == "color") {
          if (sub_node->GetAttribute("name") == "catenary") {
            status = ColorXmlHandler::ParseNode(
                sub_node, filepath,
                config.options_plot_profile.color_catenary);
          } else if (sub_node->GetAttribute("name") == "method") {
            status = ColorXmlHandler::ParseNode(
                sub_node, filepath,
                config.options_plot_profile.color_method);
          } else {
            message = FileAndLineNumber(filepath, node)
                      + "XML node isn't recognized. Skipping.";
            wxLogError(message);
            status = false;
          }
        } else if (sub_title == "scale_horizontal") {
          if (sub_content.ToLong(&value) == true) {
            config.options_plot_profile.scale_horizontal = value;
          } else {
            message = FileAndLineNumber(filepath, node)
                      + "Invalid horizontal scale. Skipping.";
            wxLogError(message);
            status = false;
          }
        } else if (sub_title == "scale_vertical") {
          if (sub_content.ToLong(&value) == true) {
            config.options_plot_profile.scale_vertical = value;
          } else {
            message = FileAndLineNumber(filepath, node)
                      + "Invalid vertical scale. Skipping.";
            wxLogError(message);
            status = false;
          }
        } else if (sub_title == "thickness_line") {
          if (sub_content.ToLong(&value) == true) {
            config.options_plot_profile.thickness_line = value;
          } else {
            message = FileAndLineNumber(filepath, node)
                      + "Invalid line thickness. Skipping.";
            wxLogError(message);
            status = false;
          }
        } else {
          message = FileAndLineNumber(filepath, node)
                    + "XML node isn't recognized. Skipping.";
          wxLogError(message);
          status = false;
        }

        sub_node = sub_node->GetNext();
      }
    } else if (title == "perspective") {
      config.perspective = content;
    } else if (title == "size_frame") {
      std::string str;
      int value;

      str = node->GetAttribute("x");
      value = std::stoi(str);
      if (400 < value) {
        config.size_frame.SetWidth(value);
      }

      str = node->GetAttribute("y");
      value = std::stoi(str);
      if (400 < value) {
        config.size_frame.SetHeight(value);
      }

      str = node->GetAttribute("is_maximized");
      if (str == "0") {
        config.is_maximized_frame = false;
      } else if (str =="1") {
        config.is_maximized_frame = true;
      }
    } else if (title == "units") {
      if (content == "Metric") {
        config.units = units::UnitSystem::kMetric;
      } else if (content == "Imperial") {
        config.units = units::UnitSystem::kImperial;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Unit system isn't recognized. Keeping default setting.";
        wxLogWarning(message);
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized. Skipping.";
      wxLogError(message);
      status = false;
    }

    node = node->GetNext();
  }

  return status;
}

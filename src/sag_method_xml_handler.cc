// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_method_xml_handler.h"

#include "appcommon/xml/point_xml_handler.h"

wxXmlNode* SagMethodXmlHandler::CreateNode(
    const SagMethod& method,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;

  // creates a node for the span root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "sag_method");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // creates type node and adds to root node
  title = "type";
  if (method.type == SagMethod::Type::kDynamometer) {
    content = "dyno";
  } else if (method.type == SagMethod::Type::kStopWatch) {
    content = "stopwatch";
  } else if (method.type == SagMethod::Type::kTransit) {
    content = "transit";
  } else {
    content = "";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates end node and adds to root node
  title = "end";
  if (method.end == SagMethod::SpanEndType::kAheadOnLine) {
    content = "ahead";
  } else if (method.end == SagMethod::SpanEndType::kBackOnLine) {
    content = "back";
  } else {
    content = "";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates position-transit node and adds to parent node
  title = "point_transit";
  if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "ft");
  } else if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "m");
  } else {
    attribute = wxXmlAttribute("", "");
  }
  node_element = Point2dDoubleXmlHandler::CreateNode(
      method.point_transit, title, attribute, 2);
  node_root->AddChild(node_element);

  // creates wave node and adds to parent node
  title = "wave_return";
  if (method.wave_return != -9999) {
    content = std::to_string(method.wave_return);
  } else {
    content == "";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SagMethodXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    SagMethod& method) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "sag_method") {
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
    return ParseNodeV1(root, filepath, method);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool SagMethodXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    SagMethod& method) {
  // variables used to parse XML node
  bool status = true;
  wxString title;
  wxString content;
  wxString message;
  long value;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "type") {
      if (content == "dyno") {
        method.type = SagMethod::Type::kDynamometer;
      } else if (content == "stopwatch") {
        method.type = SagMethod::Type::kStopWatch;
      } else if (content == "transit") {
        method.type = SagMethod::Type::kTransit;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid type.";
        wxLogError(message);
        status = false;
      }
    } else if (title == "end") {
      if (content == "") {
        // do nothing, this data may not be needed
      } else if (content == "back") {
        method.end = SagMethod::SpanEndType::kBackOnLine;
      } else if (content == "ahead") {
        method.end = SagMethod::SpanEndType::kAheadOnLine;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid span end.";
        wxLogError(message);
        status = false;
      }
    } else if (title == "point_2d") {
      const bool status_node = Point2dDoubleXmlHandler::ParseNode(
          node, filepath, method.point_transit);
      if (status_node == false) {
        status = false;
      }
    } else if (title == "wave_return") {
      if (content == "") {
        // do nothing, this data may not be needed
      } else if (content.ToLong(&value) == true) {
        method.wave_return = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid return wave.";
        wxLogError(message);
        method.wave_return = -9999;
        status = false;
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogError(message);
      status = false;
    }

    node = node->GetNext();
  }

  return status;
}

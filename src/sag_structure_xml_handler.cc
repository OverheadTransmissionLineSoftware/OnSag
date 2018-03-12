// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_structure_xml_handler.h"

#include "appcommon/xml/point_xml_handler.h"

wxXmlNode* SagStructureXmlHandler::CreateNode(
    const SagStructure& structure,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;

  // creates a node for the span root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "sag_structure");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // creates name node and adds to root node
  title = "name";
  content = structure.name;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates description-attachment node and adds to root node
  title = "attachment";
  content = structure.attachment;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates point-attachment node and adds to parent node
  title = "point_attachment";
  if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "ft");
  } else if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "m");
  } else {
    attribute = wxXmlAttribute("", "");
  }
  node_element = Point2dDoubleXmlHandler::CreateNode(
      structure.point_attachment, title, attribute, 2);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SagStructureXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    SagStructure& structure) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "sag_structure") {
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
    return ParseNodeV1(root, filepath, structure);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool SagStructureXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    SagStructure& structure) {
  // variables used to parse XML node
  bool status = true;
  wxString title;
  wxString content;
  wxString message;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "name") {
      structure.name = content;
    } else if (title == "attachment") {
      structure.attachment = content;
    } else if (title == "point_2d") {
      const bool status_node = Point2dDoubleXmlHandler::ParseNode(
          node, filepath, structure.point_attachment);
      if (status_node == false) {
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

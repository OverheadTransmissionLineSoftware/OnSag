// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_span_xml_handler.h"

#include "models/base/helper.h"

#include "onsag/sag_cable_xml_handler.h"
#include "onsag/sag_method_xml_handler.h"
#include "onsag/sag_structure_xml_handler.h"

wxXmlNode* SagSpanXmlHandler::CreateNode(
    const SagSpan& span,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;
  double value;

  // creates a node for the span root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "sag_span");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // creates description node and adds to root node
  title = "description";
  content = span.description;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates cable node and adds to parent node
  node_element = SagCableXmlHandler::CreateNode(span.cable, "", units);
  node_root->AddChild(node_element);

  // creates structure-back node and adds to parent node
  node_element = SagStructureXmlHandler::CreateNode(span.structure_back,
                                                    "back",
                                                    units);
  node_root->AddChild(node_element);

  // creates structure-ahead node and adds to parent node
  node_element = SagStructureXmlHandler::CreateNode(span.structure_ahead,
                                                    "ahead",
                                                    units);
  node_root->AddChild(node_element);

  // creates method node and adds to parent node
  node_element = SagMethodXmlHandler::CreateNode(span.method, "", units);
  node_root->AddChild(node_element);

  // creates temperature-base node and adds to parent node
  title = "temperature_base";
  value = span.temperature_base;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates temperature-interval node and adds to parent node
  title = "temperature_interval";
  value = span.temperature_interval;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates notes node and adds to parent node
  title = "notes";
  content = span.notes;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SagSpanXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    SagSpan& span) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "sag_span") {
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
    return ParseNodeV1(root, filepath, span);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool SagSpanXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    SagSpan& span) {
  // variables used to parse XML node
  bool status = true;
  wxString title;
  wxString content;
  wxString message;
  double value;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "description") {
      span.description = content;
    } else if (title == "sag_cable") {
      const bool status_node = SagCableXmlHandler::ParseNode(
          node, filepath, span.cable);
      if (status_node == false) {
        status = false;
      }
    } else if (title == "sag_structure") {
      // selects cable component type and passes off to cable component parser
      wxString name_structure = node->GetAttribute("name");

      if (name_structure == "back") {
        const bool status_node = SagStructureXmlHandler::ParseNode(
            node, filepath, span.structure_back);
        if (status_node == false) {
          status = false;
        }
      } else if (name_structure == "ahead") {
        const bool status_node = SagStructureXmlHandler::ParseNode(
            node, filepath, span.structure_ahead);
        if (status_node == false) {
          status = false;
        }
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "XML node isn't recognized.";
        wxLogError(message);
        status = false;
      }
    } else if (title == "sag_method") {
      const bool status_node = SagMethodXmlHandler::ParseNode(
          node, filepath, span.method);
      if (status_node == false) {
        status = false;
      }
    } else if (title == "temperature_base") {
      if (content.ToDouble(&value) == true) {
        span.temperature_base = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid base temperature.";
        wxLogError(message);
        span.temperature_base = -999999;
        status = false;
      }
    } else if (title == "temperature_interval") {
      if (content.ToDouble(&value) == true) {
        span.temperature_interval = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid interval temperature.";
        wxLogError(message);
        span.temperature_interval = -999999;
        status = false;
      }
    } else if (title == "notes") {
      span.notes = content;
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

// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_cable_xml_handler.h"

#include "models/base/helper.h"

wxXmlNode* SagCableXmlHandler::CreateNode(
    const SagCable& cable,
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
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "sag_cable");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // creates name node and adds to root node
  title = "name";
  content = cable.name;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates unit-weight node and adds to parent node
  title = "weight_unit";
  value = cable.weight_unit;
  content = helper::DoubleToFormattedString(value, 3);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "N/m");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lb/ft");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates tensions node
  title = "tensions";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  for (auto iter = cable.tensions.cbegin(); iter != cable.tensions.cend();
       iter++) {
    const SagCable::TensionPoint& point = *iter;
    wxXmlNode* sub_node = CreateNodeTensionPoint(point, "", units);
    node_element->AddChild(sub_node);
  }
  node_root->AddChild(node_element);

  // creates correction-creep node and adds to parent node
  title = "correction_creep";
  value = cable.correction_creep;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates correction-sag node and adds to parent node
  title = "correction_sag";
  value = cable.correction_sag;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "cm");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "in");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates scale node and adds to parent node
  title = "scale";
  value = cable.scale;
  content = helper::DoubleToFormattedString(value, 2);
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SagCableXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    SagCable& cable) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "sag_cable") {
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
    return ParseNodeV1(root, filepath, cable);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

  wxXmlNode* SagCableXmlHandler::CreateNodeTensionPoint(
    const SagCable::TensionPoint& point,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  double value = -999999;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "tension_point");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // creates temperature node and adds to root node
  title = "temperature";
  value = point.temperature;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates resistance node and adds to root node
  title = "tension_horizontal";
  value = point.tension_horizontal;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "N");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // returns root node
  return node_root;
}

 bool SagCableXmlHandler::ParseNodeTensionPoint(
    const wxXmlNode* root,
    const std::string& filepath,
    SagCable::TensionPoint& point) {
  // variables used to parse XML node
  bool status = true;
  wxString title;
  wxString content;
  double value = -999999;

  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "tension_horizontal") {
      if (content.ToDouble(&value) == true) {
        point.tension_horizontal = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid horizontal tension.";
        wxLogError(message);
        point.tension_horizontal = -999999;
        status = false;
      }
    } else if (title == "temperature") {
      if (content.ToDouble(&value) == true) {
        point.temperature = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid temperature.";
        wxLogError(message);
        point.temperature = -999999;
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

bool SagCableXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    SagCable& cable) {
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

    if (title == "name") {
      cable.name = content;
    } else if (title == "weight_unit") {
      if (content.ToDouble(&value) == true) {
        cable.weight_unit = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid unit weight.";
        wxLogError(message);
        cable.weight_unit = -999999;
        status = false;
      }
    } else if (title == "tensions") {
      // gets tension point sub-nodes
      wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        // parses resistance point node
        SagCable::TensionPoint point;
        const bool status_node = ParseNodeTensionPoint(sub_node,
                                                       filepath, point);
        if (status_node == true) {
          // adds to resistance container
          cable.tensions.push_back(point);
        } else {
          status = false;
        }

        sub_node = sub_node->GetNext();
      }
    } else if (title == "correction_creep") {
      if (content.ToDouble(&value) == true) {
        cable.correction_creep = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid creep correction.";
        wxLogError(message);
        cable.correction_creep = -999999;
        status = false;
      }
    } else if (title == "correction_sag") {
      if (content.ToDouble(&value) == true) {
        cable.correction_sag = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid sag correction.";
        wxLogError(message);
        cable.correction_sag = -999999;
        status = false;
      }
    } else if (title == "scale") {
      if (content.ToDouble(&value) == true) {
        cable.scale = value;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid scale factor.";
        wxLogError(message);
        cable.scale = -999999;
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

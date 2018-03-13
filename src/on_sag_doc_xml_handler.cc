// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/on_sag_doc_xml_handler.h"

#include "onsag/sag_span_xml_handler.h"

wxXmlNode* OnSagDocXmlHandler::CreateNode(
    const OnSagDoc& doc,
    const units::UnitSystem& units) {
  // initializes variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "on_sag_doc");
  node_root->AddAttribute("version", "1");

  // adds child nodes for parameters

  // creates sag spans node
  title = "sag_spans";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  const std::list<SagSpan>& spans = doc.spans();
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const SagSpan& span = *iter;

    // creates span node and adds to root
    node_element->AddChild(SagSpanXmlHandler::CreateNode(span, "", units));
  }
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool OnSagDocXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    OnSagDoc& doc) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "on_sag_doc") {
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
    return ParseNodeV1(root, filepath, doc);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool OnSagDocXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    OnSagDoc& doc) {
  bool status = true;
  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "sag_spans") {
      // parses each span xml node
      const wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        wxString sub_title = sub_node->GetName();
        if (sub_title == "sag_span") {
          // creates new span and parses node
          SagSpan span;
          const bool status_node = SagSpanXmlHandler::ParseNode(
              sub_node, filepath, span);
          if (status_node == false) {
            status = false;
          }

          // adds to container
          doc.AppendSpan(span);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "XML node isn't recognized. Skipping.";
          wxLogError(message);
          status = false;
        }
        sub_node = sub_node->GetNext();
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

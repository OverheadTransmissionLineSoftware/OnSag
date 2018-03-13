// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_ON_SAG_CONFIG_XML_HANDLER_H_
#define ONSAG_ON_SAG_CONFIG_XML_HANDLER_H_

#include <string>

#include "appcommon/xml/xml_handler.h"
#include "wx/xml/xml.h"

#include "onsag/on_sag_config.h"

/// \par OVERVIEW
///
/// This class parses and generates an OnSagConfig XML node. The data is
/// transferred between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
class OnSagConfigXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for the config settings.
  /// \param[in] config
  ///   The config settings.
  /// \return An XML node for the config settings.
  static wxXmlNode* CreateNode(const OnSagConfig& config);

  /// \brief Parses an XML node and populates the config settings.
  /// \param[in] root
  ///   The XML root node for the config settings.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] config
  ///   The config settings that are populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        OnSagConfig& config);

 private:
  /// \brief Parses a version 1 XML node and populates the config settings.
  /// \param[in] root
  ///   The XML root node for the config settings.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] config
  ///   The config settings that are populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          OnSagConfig& config);
};

#endif  // ONSAG_ON_SAG_CONFIG_XML_HANDLER_H_

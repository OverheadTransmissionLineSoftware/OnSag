// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGMETHODXMLHANDLER_H_
#define OTLS_ONSAG_SAGMETHODXMLHANDLER_H_

#include <string>

#include "appcommon/xml/xml_handler.h"
#include "models/base/units.h"
#include "wx/xml/xml.h"

#include "onsag/sag_method.h"

/// \par OVERVIEW
///
/// This class parses and generates a sag method XML node. The data is
/// transferred between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
///
/// \par UNIT ATTRIBUTES
///
/// This class supports attributing the child XML nodes for various unit
/// systems.
class SagMethodXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for a sag method.
  /// \param[in] method
  ///   The sag method.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the sag method.
  static wxXmlNode* CreateNode(const SagMethod& method,
                               const std::string& name,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a sag method.
  /// \param[in] root
  ///   The XML root node for the method.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] method
  ///   The sag method that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        SagMethod& method);

 private:
  /// \brief Parses a version 1 XML node and populates a method.
  /// \param[in] root
  ///   The XML root node for the method.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] method
  ///   The sag method that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          SagMethod& method);
};

#endif  // OTLS_ONSAG_SAGMETHODXMLHANDLER_H_

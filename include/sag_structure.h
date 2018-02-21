// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGSTRUCTURE_H_
#define OTLS_ONSAG_SAGSTRUCTURE_H_

#include <list>
#include <string>

#include "models/base/point.h"
#include "models/base/error_message.h"

/// \par OVERVIEW
///
/// This struct contains sagging information for an end structure.
struct SagStructure {
 public:
  /// \brief Constructor.
  SagStructure();

  /// \brief Destructor.
  ~SagStructure();

  /// \brief Validates member variables.
  /// \param[in] is_included_warnings
  ///   A flag that tightens the acceptable value range.
  /// \param[in,out] messages
  ///   A list of detailed error messages. If this is provided, any validation
  ///   errors will be appended to the list.
  /// \return A boolean value indicating status of member variables.
  bool Validate(const bool& is_included_warnings = true,
                std::list<ErrorMessage>* messages = nullptr) const;

  /// \var point_attachment
  ///   The 3D attachment coordinates.
  Point2d<double> point_attachment;

  /// \var attachment
  ///   The attachment description.
  std::string attachment;

  /// \var name
  ///   The name.
  std::string name;
};

#endif  // OTLS_ONSAG_SAGSTRUCTURE_H_

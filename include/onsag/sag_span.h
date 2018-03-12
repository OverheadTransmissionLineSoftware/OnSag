// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGSPAN_H_
#define OTLS_ONSAG_SAGSPAN_H_

#include <list>
#include <string>

#include "models/base/error_message.h"

#include "onsag/sag_cable.h"
#include "onsag/sag_method.h"
#include "onsag/sag_structure.h"

/// \par OVERVIEW
///
/// This struct contains information for a sag span.
struct SagSpan {
 public:
  /// \brief Constructor.
  SagSpan();

  /// \brief Destructor.
  ~SagSpan();

  /// \brief Validates member variables.
  /// \param[in] is_included_warnings
  ///   A flag that tightens the acceptable value range.
  /// \param[in,out] messages
  ///   A list of detailed error messages. If this is provided, any validation
  ///   errors will be appended to the list.
  /// \return A boolean value indicating status of member variables.
  bool Validate(const bool& is_included_warnings = true,
                std::list<ErrorMessage>* messages = nullptr) const;

  /// \var cable
  ///   The cable.
  SagCable cable;

  /// \var description
  ///   The title description.
  std::string description;

  /// \var method
  ///   The sagging method information.
  SagMethod method;

  /// \var notes
  ///   The notes.
  std::string notes;

  /// \var structure_ahead
  ///   The ahead-on-line structure.
  SagStructure structure_ahead;

  /// \var structure_back
  ///   The back-on-line structure.
  SagStructure structure_back;

  /// \var temperature_base
  ///   The base cable temperature when sagging.
  double temperature_base;

  /// \var temperature_interval
  ///   The temperature interval to calculate results for. This is applied above
  ///   and below the base temperature.
  double temperature_interval;
};

#endif  // OTLS_ONSAG_SAGSPAN_H_

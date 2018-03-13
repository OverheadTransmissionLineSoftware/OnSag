// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_SAG_METHOD_H_
#define ONSAG_SAG_METHOD_H_

#include <list>

#include "models/base/error_message.h"
#include "models/base/point.h"

/// \par OVERVIEW
///
/// This struct contains information for the sagging method.
struct SagMethod {
 public:
  /// \par OVERVIEW
  ///
  /// The enum contains types of sagging methods.
  enum class Type {
    kNull,
    kDynamometer,
    kStopWatch,
    kTransit
  };

  /// \par OVERVIEW
  ///
  /// This enum contains end types for sag spans.
  enum class SpanEndType {
    kNull,
    kAheadOnLine,
    kBackOnLine
  };

  /// \brief Constructor.
  SagMethod();

  /// \brief Destructor.
  ~SagMethod();

  /// \brief Validates member variables.
  /// \param[in] is_included_warnings
  ///   A flag that tightens the acceptable value range.
  /// \param[in,out] messages
  ///   A list of detailed error messages. If this is provided, any validation
  ///   errors will be appended to the list.
  /// \return A boolean value indicating status of member variables.
  bool Validate(const bool& is_included_warnings = true,
                std::list<ErrorMessage>* messages = nullptr) const;

  /// \var end
  ///   The span end type. This is only used for the 'kDynamometer' sagging
  ///   method.
  SpanEndType end;

  /// \var point_transit
  ///   The 3D coordinates of the transit. This is only used for the 'kTransit'
  ///   sagging method.
  Point2d<double> point_transit;

  /// \var type
  ///   The sagging measurement method.
  Type type;

  /// \var wave_return
  ///   The return wave number to calculate results for. This is only used for
  ///   the 'kStopWatch' sagging method.
  int wave_return;
};

#endif  // ONSAG_SAG_METHOD_H_

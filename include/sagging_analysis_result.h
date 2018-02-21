// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONGSAG_SAGGINGANALYSISRESULT_H_
#define OTLS_ONGSAG_SAGGINGANALYSISRESULT_H_

#include "models/base/point.h"
#include "models/transmissionline/catenary.h"

/// \par OVERVIEW
///
/// This represents a single sagging analysis result.
///
/// \par SAG METHOD
///
/// This struct stores results for all sagging methods for compatibility
/// reasons. An analysis will only solve for one method, so there will be
/// invalid data stored as well.
struct SaggingAnalysisResult {
  /// \var angle_transit
  ///   The transit angle. Only applicable for the 'kTransit' sagging method.
  double angle_transit;

  /// \var direction_transit
  ///   The direction the transit is oriented. Only applicable for the
  ///   'kTransit' sagging method.
  AxisDirectionType direction_transit;

  /// \var distance_target
  ///   The vertical distance from the structure attachment to the target
  ///   position. Only applicable for the 'kTransit' sagging method.
  double distance_target;

  /// \var point_target
  ///   The target point. Only applicable for the 'kTransit' sagging method.
  Point2d<double> point_target;

  /// \var catenary
  ///   The catenary. Applicable for all sagging methods.
  Catenary3d catenary;

  /// \var factor_control
  ///   The control factor. Only applicable for the 'kTransit' sagging method.
  double factor_control;

  /// \var offset_coordinates
  ///   The offset to convert catenary coordinates to span coordinates.
  ///   Applicable for all sagging methods.
  Point2d<double> offset_coordinates;

  /// \var temperature_cable
  ///   The cable temperature. Applicable for all sagging methods.
  const double* temperature_cable;

  /// \var tension_dyno
  ///   The dynamometer tension. Only applicable for the 'kDynamometer' sagging
  ///   method.
  double tension_dyno;

  /// \var time_stopwatch
  ///   The stopwatch time. Only applicable for the 'kStopwatch' sagging method.
  double time_stopwatch;
};

#endif  // OTLS_ONGSAG_SAGGINGANALYSISRESULT_H_

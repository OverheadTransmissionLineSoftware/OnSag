// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_SPAN_SAGGER_H_
#define ONSAG_SPAN_SAGGER_H_

#include <list>
#include <string>
#include <vector>

#include "models/base/error_message.h"
#include "models/base/units.h"
#include "models/sagging/dyno_sagger.h"
#include "models/sagging/stopwatch_sagger.h"
#include "models/sagging/transit_sagger.h"
#include "models/transmissionline/catenary.h"

#include "onsag/sag_cable.h"
#include "onsag/sag_method.h"
#include "onsag/sag_structure.h"

/// \todo add control factor checks

/// \par OVERVIEW
///
/// This class interfaces between the application sagging data and the
/// OTLS-Models libraries to calculate a sagging result.
///
/// \par CATENARY
///
/// A catenary is solved for based on the provided information. The catenary
/// will reflect the cable tension/position, accounting for:
/// - Creep Correction: the target temperature is reduced by the creep
///   correction.
/// - Temperature: the horizontal tension can vary with temperature. The
///   horizontal tension at the target tension is solved for using linear
///   interpolation. Future methods may use polynomial fitting for determining
///   the horizontal tension.
/// - Sag Correction: the sag increase/decrease.
///
/// \par METHOD SAGGER
///
/// This class supports the following sagging methods:
/// - transit
/// - dynamometer
/// - stopwatch
///
/// Only one sagging method can be selected at a time. The results provided by
/// this class can be tied to a specific sagging method. Check the method
/// description to see any restrictions.
class SpanSagger {
 public:
  /// \brief Constructor.
  SpanSagger();

  /// \brief Destructor.
  ~SpanSagger();

  /// \brief Gets the transit angle.
  /// \return The transit angle.
  /// This method will only return a valid answer if the transit sagging
  /// method is used.
  double AngleTransit() const;

  /// \brief Gets the catenary.
  /// \return The catenary.
  Catenary3d Catenary() const;

  /// \brief Gets the transit direction along the x-axis.
  /// \return The transit direction along the x-axis.
  AxisDirectionType DirectionTransit() const;

  /// \brief Gets the vertical distance from the attachment to the target.
  /// \return The vertical distance from the attachment to the target.
  double DistanceAttachmentToTarget() const;

  /// \brief Gets the control factor.
  /// \return The control factor.
  /// This method will only return a valid answer if the transit sagging
  /// method is used.
  double FactorControl() const;

  /// \brief Gets the target point.
  /// \return The target point.
  /// This method will only return a valid answer if the transit sagging
  /// method is used.
  Point2d<double> PointTarget() const;

  /// \brief Gets the dynamometer tension.
  /// \return The dynamometer tension.
  /// This method will only return a valid answer if the dynamometer sagging
  /// method is used.
  double TensionDyno() const;

  /// \brief Gets the stopwatch time.
  /// \return The stopwatch time.
  /// This method will only return a valid answer if the stopwatch sagging
  /// method is used.
  double TimeStopwatch() const;

  /// \brief Validates member variables.
  /// \param[in] is_included_warnings
  ///   A flag that tightens the acceptable value range.
  /// \param[in,out] messages
  ///   A list of detailed error messages. If this is provided, any validation
  ///   errors will be appended to the list.
  /// \return A boolean value indicating status of member variables.
  bool Validate(const bool& is_included_warnings = true,
                std::list<ErrorMessage>* messages = nullptr) const;

  /// \brief Gets the cable.
  /// \return The cable.
  const SagCable* cable() const;

  /// \brief Gets the method.
  /// \return The method.
  const SagMethod* method() const;

  /// \brief Sets the cable.
  /// \param[in] cable
  ///   The cable.
  void set_cable(const SagCable* cable);

  /// \brief Sets the method.
  /// \param[in] method
  ///   The method.
  void set_method(const SagMethod* method);

  /// \brief Sets the ahead structure.
  /// \param[in] structure_ahead
  ///   The ahead structure.
  void set_structure_ahead(const SagStructure* structure_ahead);

  /// \brief Sets the back structure.
  /// \param[in] structure_back
  ///   The ahead structure.
  void set_structure_back(const SagStructure* structure_back);

  /// \brief Sets the temperature.
  /// \param[in] temperature
  ///   The temperature.
  void set_temperature(const double* temperature);

  /// \brief Sets the units.
  /// \param[in] units
  ///   The unit system.
  void set_units(const units::UnitSystem& units);

  /// \brief Gets the ahead structure.
  /// \return The ahead structure.
  const SagStructure* structure_ahead() const;

  /// \brief Gets the back structure.
  /// \return The back structure.
  const SagStructure* structure_back() const;

  /// \brief Gets the temperature.
  /// \return The temperature.
  const double* temperature() const;

  /// \brief Gets the units.
  /// \return The units.
  units::UnitSystem units() const;

 private:
  /// \brief Determines if class is updated.
  /// \return A boolean indicating if class is updated.
  bool IsUpdated() const;

  /// \brief Determines if the control factor is valid.
  /// \param[in] factor_control
  ///   The control factor.
  /// \return If the control factor is valid.
  bool IsValidControlFactor(const double& factor_control) const;

  /// \brief Gets the polynomial fitted horizontal tension.
  /// \param[in] points
  ///   The points to use for the polynomial fit.
  /// \param[in] temperature
  ///   The target temperature.
  /// \return The polynomial fitted horizontal tension.
  double TensionHorizontalPolynomialFitted(
      const std::list<const SagCable::TensionPoint*>& points,
      const double& temperature) const;

  /// \brief Updates cached member variables and modifies control variables if
  ///   update is required.
  /// \return A boolean indicating if class updates completed successfully.
  bool Update() const;

  /// \brief Updates the catenary.
  /// \return The success status of the update.
  bool UpdateCatenary() const;

  /// \brief Updates the target position.
  /// \return The success status of the update.
  bool UpdatePointTarget() const;

  /// \brief Updates the sagger that is specified for use.
  /// \return The success status of the update.
  bool UpdateSagger() const;

  /// \var cable_
  ///   The cable.
  const SagCable* cable_;

  /// \var catenary_
  ///   The catenary for the specified cable temperature.
  mutable Catenary3d catenary_;

  /// \var is_updated_catenary_
  ///   An indicator that tells if the class is updated.
  mutable bool is_updated_catenary_;

  /// \var is_updated_sagger_
  ///   An indicator that tells if the specific sagger is updated.
  mutable bool is_updated_sagger_;

  /// \var method_
  ///   The sag method.
  const SagMethod* method_;

  /// \var point_target_
  ///   The target point.
  mutable Point2d<double> point_target_;

  /// \var points_control_min_
  ///   The minimum control factor points. The x coordinate is the elevation
  ///   factor (i.e. vertical spacing divided by the horizontal spacing). The y
  ///   coordinate is the minimum allowable control factor (i.e. sag at tangency
  ///   point divided by the maximum sag).
  std::vector<Point2d<double>> points_control_min_;

  /// \var sagger_dyno_
  ///   The dyno sagger. This is only updated when the dynamometer sagging
  ///   method is specified.
  mutable DynoSagger sagger_dyno_;

  /// \var sagger_stopwatch_
  ///   The stopwatch sagger. This is only updated when the stopwatch sagging
  ///   method is specified.
  mutable StopwatchSagger sagger_stopwatch_;

  /// \var sagger_transit_
  ///   The transit sagger. This is only updated when the transit sagging
  ///   method is specified.
  mutable TransitSagger sagger_transit_;

  /// \var structure_ahead_
  ///   The ahead-on-line structure.
  const SagStructure* structure_ahead_;

  /// \var structure_back_
  ///   The back-on-line structure.
  const SagStructure* structure_back_;

  /// \var temperature_
  ///   The cable temperature.
  const double* temperature_;

  /// \var units_
  ///   The unit system.
  units::UnitSystem units_;
};

#endif  // ONSAG_SPAN_SAGGER_H_

// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/span_sagger.h"

#include <cmath>

#include "models/base/helper.h"
#include "models/base/polynomial.h"
#include "models/transmissionline/catenary_solver.h"

SpanSagger::SpanSagger() {
  cable_ = nullptr;
  method_ = nullptr;
  structure_ahead_ = nullptr;
  structure_back_ = nullptr;
  temperature_ = nullptr;
  units_ = units::UnitSystem::kNull;

  is_updated_catenary_ = false;
  is_updated_sagger_ = false;

  points_control_min_.resize(16, Point2d<double>());
  points_control_min_[0] = Point2d<double>(0.00, 0.750);
  points_control_min_[1] = Point2d<double>(0.05, 0.750);
  points_control_min_[2] = Point2d<double>(0.10, 0.850);
  points_control_min_[3] = Point2d<double>(0.15, 0.910);
  points_control_min_[4] = Point2d<double>(0.20, 0.940);
  points_control_min_[5] = Point2d<double>(0.25, 0.960);
  points_control_min_[6] = Point2d<double>(0.30, 0.970);
  points_control_min_[7] = Point2d<double>(0.35, 0.980);
  points_control_min_[8] = Point2d<double>(0.40, 0.985);
  points_control_min_[9] = Point2d<double>(0.45, 0.990);
  points_control_min_[10] = Point2d<double>(0.50, 0.992);
  points_control_min_[11] = Point2d<double>(0.55, 0.994);
  points_control_min_[12] = Point2d<double>(0.60, 0.996);
  points_control_min_[13] = Point2d<double>(0.65, 0.997);
  points_control_min_[14] = Point2d<double>(0.70, 0.998);
  points_control_min_[15] = Point2d<double>(0.75, 0.999);
}

SpanSagger::~SpanSagger() {
}

double SpanSagger::AngleTransit() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return -999999;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kTransit) {
    return sagger_transit_.AngleLow();
  } else {
    return -999999;
  }
}

Catenary3d SpanSagger::Catenary() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return Catenary3d();
  }

  return catenary_;
}

AxisDirectionType SpanSagger::DirectionTransit() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return AxisDirectionType::kNull;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kTransit) {
    if (method_->point_transit.x < point_target_.x) {
      return AxisDirectionType::kPositive;
    } else if (point_target_.x < method_->point_transit.x) {
      return AxisDirectionType::kNegative;
    } else {
      return AxisDirectionType::kNull;
    }
  } else {
    return AxisDirectionType::kNull;
  }
}

double SpanSagger::DistanceAttachmentToTarget() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return -999999;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kTransit) {
    // gets the direction
    AxisDirectionType direction = DirectionTransit();

    if (direction == AxisDirectionType::kNegative) {
      return structure_back_->point_attachment.y - point_target_.y;
    } else if (direction == AxisDirectionType::kPositive) {
      return structure_ahead_->point_attachment.y - point_target_.y;
    } else {
      return -999999;
    }
  } else {
    return -999999;
  }
}

double SpanSagger::FactorControl() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return -999999;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kTransit) {
    return sagger_transit_.FactorControl();
  } else {
    return -999999;
  }
}

Point2d<double> SpanSagger::PointTarget() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return Point2d<double>();
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kTransit) {
    return point_target_;
  } else {
    return Point2d<double>();
  }
}

double SpanSagger::SpeedWave() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return -999999;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kStopWatch) {
    return sagger_stopwatch_.VelocityWave();
  } else {
    return -999999;
  }
}

double SpanSagger::TensionDyno() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return -999999;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kDynamometer) {
    return sagger_dyno_.Tension();
  } else {
    return -999999;
  }
}

double SpanSagger::TimeStopwatch() const {
  // updates class if necessary
  if ((IsUpdated() == false) && (Update() == false)) {
    return -999999;
  }

  // returns valid answer if method type is correct
  if (method_->type == SagMethod::Type::kStopWatch) {
    return sagger_stopwatch_.TimeReturn(method_->wave_return);
  } else {
    return -999999;
  }
}

/// This method depends on the most of the data being checked in the
/// SagSpan::Validate() method.
bool SpanSagger::Validate(const bool& /**is_included_warnings**/,
                          std::list<ErrorMessage>* messages) const {
  // initializes
  bool is_valid = true;
  ErrorMessage message;
  message.title = "SPAN SAGGER";

  // checks cable
  if (cable_ == nullptr) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid sag cable";
      messages->push_back(message);
    }
  }

  // checks method
  if (method_ == nullptr) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid sag method";
      messages->push_back(message);
    }
  }

  // checks structure-ahead
  if (structure_ahead_ == nullptr) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid ahead structure";
      messages->push_back(message);
    }
  }

  // checks structure-back
  if (method_ == nullptr) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid back structure";
      messages->push_back(message);
    }
  }

  // checks temperature
  if (temperature_ == nullptr) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid temperature";
      messages->push_back(message);
    }
  }

  // returns if errors are present
  if (is_valid == false) {
    return false;
  }

  // compares temperature to cable tension points
  if (*temperature_ < cable_->tensions.front().temperature) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Target temperature is less than all cable "
                            "temperature-tension points";
      messages->push_back(message);
    }
  } else if (cable_->tensions.back().temperature < *temperature_) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Target temperature is greater than all cable "
                            "temperature-tension points";
      messages->push_back(message);
    }
  }

  // validates update process
  if (Update() == false) {
    is_valid = false;
    if ((messages != nullptr) && (is_updated_catenary_ == false)) {
      message.description = "Couldn't solve for a catenary.";
      messages->push_back(message);
    } else if ((messages != nullptr) && (is_updated_sagger_ == false)) {
      // can safely display transit message here because the other sagging
      // methods should pass if a catenary is solved
      message.description = "Couldn't solve for a sagging solution. Try "
                            "relocating transit.";
      messages->push_back(message);
    }

    return is_valid;
  }

  return is_valid;
}

const SagCable* SpanSagger::cable() const {
  return cable_;
}

const SagMethod* SpanSagger::method() const {
  return method_;
}

void SpanSagger::set_cable(const SagCable* cable) {
  cable_ = cable;
  is_updated_catenary_ = false;
  is_updated_sagger_ = false;
}

void SpanSagger::set_method(const SagMethod* method) {
  method_ = method;
  is_updated_catenary_ = false;
  is_updated_sagger_ = false;
}

void SpanSagger::set_structure_ahead(const SagStructure* structure_ahead) {
  structure_ahead_ = structure_ahead;
  is_updated_catenary_ = false;
  is_updated_sagger_ = false;
}

void SpanSagger::set_structure_back(const SagStructure* structure_back) {
  structure_back_ = structure_back;
  is_updated_catenary_ = false;
  is_updated_sagger_ = false;
}

void SpanSagger::set_temperature(const double* temperature) {
  temperature_ = temperature;
  is_updated_catenary_ = false;
  is_updated_sagger_ = false;
}

void SpanSagger::set_units(const units::UnitSystem& units) {
  units_ = units;
  is_updated_sagger_ = false;
}

const SagStructure* SpanSagger::structure_ahead() const {
  return structure_ahead_;
}

const SagStructure* SpanSagger::structure_back() const {
  return structure_back_;
}

const double* SpanSagger::temperature() const {
  return temperature_;
}

units::UnitSystem SpanSagger::units() const {
  return units_;
}

bool SpanSagger::IsUpdated() const {
  return (is_updated_catenary_ == true) && (is_updated_sagger_ == true);
}

bool SpanSagger::IsValidControlFactor(const double& factor_control) const {
  // calculates the elevation factor (x-axis)
  const Vector3d& spacing = catenary_.spacing_endpoints();
  const double kFactorElevation = std::abs(spacing.z() / spacing.x());

  // searches the minimum control points list to find boundary index(es)
  unsigned int i = 0;
  while (i < points_control_min_.size()) {
    const Point2d<double>& point = points_control_min_[i];

    // checks if point exceeds elevation factor
    if (kFactorElevation < point.x) {
      break;
    } else {
      i++;
    }
  }

  // checks index
  if (i == 0) {
    return false;
  } else if (i == points_control_min_.size()) {
    return false;
  }

  // assigns boundary points
  const Point2d<double>& point_start = points_control_min_[i - 1];
  const Point2d<double>& point_end = points_control_min_[i];

  // uses linear interpolation to determine the minimum control factor
  Point2d<double> point_min;
  point_min.x = factor_control;
  point_min.y = helper::LinearY(point_start.x, point_start.y,
                                point_end.x, point_end.y, point_min.x);

  // compares and returns
  return point_min.y <= factor_control;
}

double SpanSagger::TensionHorizontalPolynomialFitted(
    const std::list<const SagCable::TensionPoint*>& points,
    const double& temperature) const {
  // determines shift to absolute scale
  double temperature_shift = 0;
  if (units_ == units::UnitSystem::kImperial) {
    temperature_shift = units::ConvertTemperature(
        0,
        units::TemperatureConversionType::kFahrenheitToRankine);
  } else if (units_ == units::UnitSystem::kMetric) {
    temperature_shift = units::ConvertTemperature(
        0,
        units::TemperatureConversionType::kCelsiusToKelvin);
  } else {
    return -999999;
  }

  // creates a duplicated list of points
  // converts to x-y coordinates (x=temperature, y=tension)
  // x coordinates use absolute temperature scale
  std::vector<Point2d<double>> pts(3);
  for (auto iter = points.cbegin(); iter != points.cend(); iter++) {
    const SagCable::TensionPoint* point = *iter;

    Point2d<double> pt;
    pt.x = point->temperature + temperature_shift;
    pt.y = point->tension_horizontal;

    const int i = std::distance(points.cbegin(), iter);
    pts[i] = pt;
  }

  // converts target temperature to absolute scale
  const double x_target = temperature + temperature_shift;

  /// \todo this method needs updated to use a better implemented polynomial
  ///   fitting method. See here:
  /// https://en.wikipedia.org/wiki/Newton_polynomial

  // calculates constants to simplify expressions
  const double k1 = (pts[0].y - pts[2].y) / std::pow(pts[0].x, 2);
  const double k2 = (pts[2].x - pts[0].x) / std::pow(pts[0].x, 2);
  const double k3 = 1 - (std::pow(pts[2].x, 2) / std::pow(pts[0].x, 2));
  const double k4 = (std::pow(pts[2].x, 2) - std::pow(pts[1].x, 2)) / pts[1].x;
  const double k5 = (pts[1].y - pts[2].y) / pts[1].x;
  const double k6 = 1 - (pts[2].x / pts[1].x);

  // calculate coefficients of parabolic interpolating polynomial
  std::vector<double> coefficients(3);
  coefficients[1] = ((k5 * k3) + (k1 * k4)) / ((k6 * k3) - (k2 * k4));
  coefficients[2] = (k1 + (coefficients[1] * k2)) / k3;
  coefficients[0] = pts[2].y - (coefficients[2] * std::pow(pts[2].x, 2))
                    - (coefficients[1] * pts[2].x);

  // creates a polynomial from the coefficients
  Polynomial polynomial;
  polynomial.set_coefficients(&coefficients);

  // checks for negative polynomial concavity, which is related to how a
  // catenary lets off tension as temperature increases
  Polynomial derivative_1 = polynomial.Derivative();
  Polynomial derivative_2 = derivative_1.Derivative();
  const double kConcavity = derivative_2.Y(x_target);
  if (0 < helper::Round(kConcavity, 4)) {
    return -999999;
  }

  // calculates the horizontal tension for the target temp
  return polynomial.Y(x_target);
}

bool SpanSagger::Update() const {
  // updates the catenary
  is_updated_catenary_ = UpdateCatenary();
  if (is_updated_catenary_ == false) {
    return false;
  }

  // updates sagging result
  is_updated_sagger_ = UpdateSagger();
  if (is_updated_sagger_ == false) {
    return false;
  }

  // if it reaches this point, update was successful
  return true;
}

bool SpanSagger::UpdateCatenary() const {
  // adjusts temperature based on creep correction
  const double temperature = *temperature_ - cable_->correction_creep;

  // gets min and max temp/tension point
  const SagCable::TensionPoint& point_min = cable_->tensions.front();
  const SagCable::TensionPoint& point_max = cable_->tensions.back();

  // determines if the tension point is valid
  if (temperature < point_min.temperature) {
    return false;
  } else if (point_max.temperature < temperature) {
    return false;
  }

  // gets polynomial fit points
  // there are only 5 points total, and 3 are needed for polynomial fit
  // checks against median point and selects points 0-2 or 2-4
  std::list <const SagCable::TensionPoint*> points;
  const SagCable::TensionPoint& point_median =
      *std::next(cable_->tensions.cbegin(), 2);
  if (temperature <= point_median.temperature) {
    points.push_back(&(*std::next(cable_->tensions.cbegin(), 0)));
    points.push_back(&(*std::next(cable_->tensions.cbegin(), 1)));
    points.push_back(&(*std::next(cable_->tensions.cbegin(), 2)));
  } else if (point_median.temperature < temperature) {
    points.push_back(&(*std::next(cable_->tensions.cbegin(), 2)));
    points.push_back(&(*std::next(cable_->tensions.cbegin(), 3)));
    points.push_back(&(*std::next(cable_->tensions.cbegin(), 4)));
  }

  // interpolates to find the tension
  const double tension_horizontal =
      TensionHorizontalPolynomialFitted(points, temperature);
  if (tension_horizontal == -999999) {
    return false;
  }

  // solves for the catenary spacing
  Vector3d spacing;
  spacing.set_x(structure_ahead_->point_attachment.x
                - structure_back_->point_attachment.x);
  spacing.set_y(0);
  spacing.set_z(structure_ahead_->point_attachment.y
                - structure_back_->point_attachment.y);

  // solves for unit weight
  Vector3d weight_unit;
  weight_unit.set_x(0);
  weight_unit.set_y(0);
  weight_unit.set_z(cable_->weight_unit);

  // builds a catenary
  catenary_.set_direction_transverse(AxisDirectionType::kPositive);
  catenary_.set_spacing_endpoints(spacing);
  catenary_.set_tension_horizontal(tension_horizontal);
  catenary_.set_weight_unit(weight_unit);

  // accounts for sag correction
  if (cable_->correction_sag != 0) {
    CatenarySolver solver;
    solver.set_spacing_endpoints(catenary_.spacing_endpoints());
    solver.set_type_target(CatenarySolver::TargetType::kSag);
    solver.set_value_target(catenary_.Sag() + cable_->correction_sag);
    solver.set_weight_unit(catenary_.weight_unit());

    if (solver.Validate() == true) {
      catenary_ = solver.Catenary();
    } else {
      return false;
    }
  }

  // returns catenary status
  return catenary_.Validate(false, nullptr);
}

bool SpanSagger::UpdatePointTarget() const {
  // shifts coordinates to match external
  Point3d<double> point_target = sagger_transit_.PointTarget();
  point_target.x += structure_back_->point_attachment.x;
  point_target.z += structure_back_->point_attachment.y;

  // converts 3d point to 2d
  point_target_.x = point_target.x;
  point_target_.y = point_target.z;

  return true;
}

bool SpanSagger::UpdateSagger() const {
  // intializes all saggers and cached data
  sagger_dyno_ = DynoSagger();
  sagger_stopwatch_ = StopwatchSagger();
  sagger_transit_ = TransitSagger();
  point_target_ = Point2d<double>();

  // selects the sagger to update based on sagging method
  if (method_->type == SagMethod::Type::kDynamometer) {
    sagger_dyno_.set_catenary(catenary_);

    if (method_->end == SagMethod::SpanEndType::kAheadOnLine) {
      sagger_dyno_.set_location(DynoSagger::SpanEndLocation::kAhead);
    } else if (method_->end == SagMethod::SpanEndType::kBackOnLine) {
      sagger_dyno_.set_location(DynoSagger::SpanEndLocation::kBack);
    } else {
      return false;
    }

    return sagger_dyno_.Validate(false, nullptr);

  } else if (method_->type == SagMethod::Type::kStopWatch) {
    sagger_stopwatch_.set_catenary(catenary_);
    sagger_stopwatch_.set_units(units_);

    return sagger_stopwatch_.Validate(false, nullptr);

  } else if (method_->type == SagMethod::Type::kTransit) {
    bool status = false;

    // adjusts the transit point to fit the sagger coordinate system
    Point3d<double> point;
    point.x = method_->point_transit.x
              - structure_back_->point_attachment.x;
    point.y = 0;
    point.z = method_->point_transit.y
              - structure_back_->point_attachment.y;

    // updates the sagger
    sagger_transit_.set_catenary(catenary_);
    sagger_transit_.set_point_transit(point);

    status = sagger_transit_.Validate();
    if (status == false) {
      return false;
    }

    // checks the control factor
    status = IsValidControlFactor(sagger_transit_.FactorControl());
    if (status == false) {
      return false;
    }

    // updates target point
    UpdatePointTarget();

    return true;
  } else {
    return false;
  }
}

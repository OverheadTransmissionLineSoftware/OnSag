// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "sag_method_unit_converter.h"

void SagMethodUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& /**system**/,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    SagMethod& /**method**/) {
  if (style_from == style_to) {
    return;
  }

  // nothing to convert
}

void SagMethodUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    SagMethod& method) {
  if (system_from == system_to) {
    return;
  }

  // converts unit system for sag method
  if (system_to == units::UnitSystem::kMetric) {
    method.point_transit.x = units::ConvertLength(
      method.point_transit.x,
      units::LengthConversionType::kFeetToMeters);
    method.point_transit.y = units::ConvertLength(
      method.point_transit.y,
      units::LengthConversionType::kFeetToMeters);

  } else if (system_to == units::UnitSystem::kImperial) {
    method.point_transit.x = units::ConvertLength(
      method.point_transit.x,
      units::LengthConversionType::kMetersToFeet);
    method.point_transit.y = units::ConvertLength(
      method.point_transit.y,
      units::LengthConversionType::kMetersToFeet);
  }
}

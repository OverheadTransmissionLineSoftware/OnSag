// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_structure_unit_converter.h"

void SagStructureUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& /**system**/,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    SagStructure& /**structure**/) {
  if (style_from == style_to) {
    return;
  }

  // nothing to convert
}

void SagStructureUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    SagStructure& structure) {
  if (system_from == system_to) {
    return;
  }

  // converts unit system for sag structure
  if (system_to == units::UnitSystem::kMetric) {
    structure.point_attachment.x = units::ConvertLength(
      structure.point_attachment.x,
      units::LengthConversionType::kFeetToMeters);
    structure.point_attachment.y = units::ConvertLength(
      structure.point_attachment.y,
      units::LengthConversionType::kFeetToMeters);

  } else if (system_to == units::UnitSystem::kImperial) {
    structure.point_attachment.x = units::ConvertLength(
      structure.point_attachment.x,
      units::LengthConversionType::kMetersToFeet);
    structure.point_attachment.y = units::ConvertLength(
      structure.point_attachment.y,
      units::LengthConversionType::kMetersToFeet);
  }
}

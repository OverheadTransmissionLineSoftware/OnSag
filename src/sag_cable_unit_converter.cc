// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_cable_unit_converter.h"

void SagCableUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    SagCable& cable) {
  if (style_from == style_to) {
    return;
  }

  // converts unit style for sag cable
  if (system == units::UnitSystem::kMetric) {
    if (style_to == units::UnitStyle::kConsistent) {
      cable.correction_sag = units::ConvertLength(
          cable.correction_sag,
          units::LengthConversionType::kCentimetersToMeters);
    } else if (style_to == units::UnitStyle::kDifferent) {
      cable.correction_sag = units::ConvertLength(
          cable.correction_sag,
          units::LengthConversionType::kMetersToCentimeters);
    }
  } else if (system == units::UnitSystem::kImperial) {
    if (style_to == units::UnitStyle::kConsistent) {
      cable.correction_sag = units::ConvertLength(
          cable.correction_sag,
          units::LengthConversionType::kInchesToFeet);
    } else if (style_to == units::UnitStyle::kDifferent) {
      cable.correction_sag = units::ConvertLength(
          cable.correction_sag,
          units::LengthConversionType::kFeetToInches);
    }
  }
}

void SagCableUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    SagCable& cable) {
  if (system_from == system_to) {
    return;
  }

  // converts unit system for sag cable
  if (system_to == units::UnitSystem::kMetric) {
    cable.correction_creep = units::ConvertTemperature(
      cable.correction_creep,
      units::TemperatureConversionType::kRankineToKelvin, 1, true);

    cable.correction_sag = units::ConvertLength(
      cable.correction_sag,
      units::LengthConversionType::kFeetToMeters);

    for (auto iter = cable.tensions.begin(); iter != cable.tensions.end();
         iter++) {
      SagCable::TensionPoint& point = *iter;

      point.temperature = units::ConvertTemperature(
        point.temperature,
        units::TemperatureConversionType::kFahrenheitToCelsius, 1, true);
      point.tension_horizontal = units::ConvertForce(
        point.tension_horizontal,
        units::ForceConversionType::kPoundsToNewtons);
    }

    cable.weight_unit = units::ConvertForce(
      cable.weight_unit,
      units::ForceConversionType::kPoundsToNewtons, 1, true);
    cable.weight_unit = units::ConvertLength(
      cable.weight_unit,
      units::LengthConversionType::kFeetToMeters, 1, false);

  } else if (system_to == units::UnitSystem::kImperial) {
    cable.correction_creep = units::ConvertTemperature(
      cable.correction_creep,
      units::TemperatureConversionType::kKelvinToRankine, 1, true);

    cable.correction_sag = units::ConvertLength(
      cable.correction_sag,
      units::LengthConversionType::kMetersToFeet);

    for (auto iter = cable.tensions.begin(); iter != cable.tensions.end();
         iter++) {
      SagCable::TensionPoint& point = *iter;

      point.temperature = units::ConvertTemperature(
        point.temperature,
        units::TemperatureConversionType::kCelsiusToFahrenheit, 1, true);
      point.tension_horizontal = units::ConvertForce(
        point.tension_horizontal,
        units::ForceConversionType::kNewtonsToPounds);
    }

    cable.weight_unit = units::ConvertForce(
      cable.weight_unit,
      units::ForceConversionType::kNewtonsToPounds, 1, true);
    cable.weight_unit = units::ConvertLength(
      cable.weight_unit,
      units::LengthConversionType::kMetersToFeet, 1, false);
  }
}

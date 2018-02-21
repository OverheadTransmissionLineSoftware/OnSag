// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "sag_span_unit_converter.h"

#include "sag_cable_unit_converter.h"
#include "sag_method_unit_converter.h"
#include "sag_structure_unit_converter.h"

void SagSpanUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    SagSpan& span) {
  if (style_from == style_to) {
    return;
  }

  // converts unit style for sag span
  SagCableUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                          span.cable);

  SagMethodUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                            span.method);

  SagStructureUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                              span.structure_ahead);

  SagStructureUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                              span.structure_back);
}

void SagSpanUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    SagSpan& span) {
  if (system_from == system_to) {
    return;
  }

  // converts unit system for sag span
  if (system_to == units::UnitSystem::kMetric) {
    span.temperature_base = units::ConvertTemperature(
      span.temperature_base,
      units::TemperatureConversionType::kFahrenheitToCelsius, 1, true, true);

    span.temperature_interval = units::ConvertTemperature(
      span.temperature_interval,
      units::TemperatureConversionType::kFahrenheitToCelsius, 1, true, false);

  } else if (system_to == units::UnitSystem::kImperial) {
    span.temperature_base = units::ConvertTemperature(
      span.temperature_base,
      units::TemperatureConversionType::kCelsiusToFahrenheit, 1, true, true);

    span.temperature_interval = units::ConvertTemperature(
      span.temperature_interval,
      units::TemperatureConversionType::kCelsiusToFahrenheit, 1, true, false);
  }

  SagCableUnitConverter::ConvertUnitSystem(system_from, system_to, span.cable);

  SagMethodUnitConverter::ConvertUnitSystem(system_from, system_to,
                                            span.method);

  SagStructureUnitConverter::ConvertUnitSystem(system_from, system_to,
                                               span.structure_ahead);

  SagStructureUnitConverter::ConvertUnitSystem(system_from, system_to,
                                              span.structure_back);
}

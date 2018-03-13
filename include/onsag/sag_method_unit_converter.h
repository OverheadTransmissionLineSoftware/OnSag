// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_SAG_METHOD_UNIT_CONVERTER_H_
#define ONSAG_SAG_METHOD_UNIT_CONVERTER_H_

#include "models/base/units.h"

#include "onsag/sag_method.h"

/// \par OVERVIEW
///
/// This class converts a sag method between unit systems as well as unit
/// styles.
class SagMethodUnitConverter {
 public:
  /// \brief Converts between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] method
  ///   The method to be converted.
  /// The 'different' style units are as follows:
  ///  - position_transit = [m or ft]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const units::UnitStyle& style_from,
                               const units::UnitStyle& style_to,
                               SagMethod& method);

  /// \brief Converts between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// \param[in,out] method
  ///   The method to be converted.
  /// This function requires the data in a 'consistent' unit style.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                SagMethod& method);
};

#endif  // ONSAG_SAG_METHOD_UNIT_CONVERTER_H_

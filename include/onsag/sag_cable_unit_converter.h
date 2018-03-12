// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGCABLEUNITCONVERTER_H_
#define OTLS_ONSAG_SAGCABLEUNITCONVERTER_H_

#include "models/base/units.h"

#include "onsag/sag_cable.h"

/// \par OVERVIEW
///
/// This class converts a sag cable between unit systems as well as unit styles.
class SagCableUnitConverter {
 public:
  /// \brief Converts between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] cable
  ///   The cable to be converted.
  /// The 'different' style units are as follows:
  ///  - correction_creep = [degC or degF]
  ///  - correction_sag = [cm or in]
  ///  - tensions = [degC,N or degF,lbs]
  ///  - weight_unit = [N/m or lb/ft]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const units::UnitStyle& style_from,
                               const units::UnitStyle& style_to,
                               SagCable& cable);

  /// \brief Converts between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// \param[in,out] cable
  ///   The cable to be converted.
  /// This function requires the data in a 'consistent' unit style.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                SagCable& cable);
};

#endif  // OTLS_ONSAG_SAGCABLEUNITCONVERTER_H_

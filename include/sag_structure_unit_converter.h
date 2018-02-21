// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGSTRUCTUREUNITCONVERTER_H_
#define OTLS_ONSAG_SAGSTRUCTUREUNITCONVERTER_H_

#include "models/base/units.h"

#include "sag_structure.h"

/// \par OVERVIEW
///
/// This class converts a sag structure between unit systems as well as unit
/// styles.
class SagStructureUnitConverter {
 public:
  /// \brief Converts between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] structure
  ///   The structure to be converted.
  /// The 'different' style units are as follows:
  ///  - position_attachment = [m or ft]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const units::UnitStyle& style_from,
                               const units::UnitStyle& style_to,
                               SagStructure& structure);

  /// \brief Converts between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// \param[in,out] structure
  ///   The structure to be converted.
  /// This function requires the data in a 'consistent' unit style.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                SagStructure& structure);
};

#endif  // OTLS_ONSAG_SAGSTRUCTUREUNITCONVERTER_H_

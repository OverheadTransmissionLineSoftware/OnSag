// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_SAGCABLE_H_
#define OTLS_ONSAG_SAGCABLE_H_

#include <list>
#include <string>

#include "onsag/models/base/error_message.h"

/// \par OVERVIEW
///
/// This struct contains sagging information for the cable.
struct SagCable {
 public:
  /// \par OVERVIEW
  ///
  /// This struct contains a temperature-tension point of the cable.
  struct TensionPoint {
   public:
    /// \var temperature
    ///   The temperature of the cable.
    double temperature;

    /// \var tension_horizontal
    ///   The horizontal tension of the cable/catenary.
    double tension_horizontal;
  };

  /// \brief Constructor.
  SagCable();

  /// \brief Destructor.
  ~SagCable();

  /// \brief Validates member variables.
  /// \param[in] is_included_warnings
  ///   A flag that tightens the acceptable value range.
  /// \param[in,out] messages
  ///   A list of detailed error messages. If this is provided, any validation
  ///   errors will be appended to the list.
  /// \return A boolean value indicating status of member variables.
  bool Validate(const bool& is_included_warnings = true,
                std::list<ErrorMessage>* messages = nullptr) const;

  /// \var correction_creep
  ///   The correction that accounts for long-term creep of the cable. This is
  ///   expressed as a temperature, and is subtracted from the nominal
  ///   temperature.
  double correction_creep;

  /// \var correction_sag
  ///   The correction that accounts for the sag difference of the cable when
  ///   suspended in travelers. This is added to the nominal sag of the cable.
  double correction_sag;

  /// \var name
  ///   The name.
  std::string name;

  /// \var scale
  ///   The factor to scale the tensions by. This is typically done for
  ///   pre-stressing the cable.
  double scale;

  /// \var tensions
  ///   The tensions that the cable is installed to.
  std::list<TensionPoint> tensions;

  /// \var weight_unit
  ///   The weight per linear length.
  double weight_unit;
};

#endif  // OTLS_ONSAG_SAGCABLE_H_

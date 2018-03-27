// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_cable.h"

SagCable::SagCable() {
  correction_creep = -999999;
  correction_sag = -999999;
  name = "";
  scale = -999999;
  weight_unit = -999999;
}

SagCable::~SagCable() {
}

bool SagCable::Validate(const bool& is_included_warnings,
                        std::list<ErrorMessage>* messages) const {
  // initializes
  bool is_valid = true;
  ErrorMessage message;
  message.title = "SAG CABLE";

  // validates correction-creep
  if ((correction_creep < 0) || (100 < correction_creep)) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid creep correction";
      messages->push_back(message);
    }
  }

  // validates correction-sag
  if ((correction_sag < -100) || (100 < correction_sag)) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid sag correction";
      messages->push_back(message);
    }
  }

  // validates scale
  if ((scale <= 0) || (10 < scale)) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid scale factor";
      messages->push_back(message);
    }
  }

  // validates tensions
  const SagCable::TensionPoint* point_prev = nullptr;
  for (auto iter = tensions.cbegin(); iter != tensions.cend(); iter++) {
    const SagCable::TensionPoint& point = *iter;

    // checks temperature
    if ((point.temperature < -50) || (150 < point.temperature)) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid tension point, temperature is out of"
                              " range";
        messages->push_back(message);
      }
    }

    // checks tension
    if (point.tension_horizontal <= 0) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid tension point, tension is out of range";
        messages->push_back(message);
      }
    }

    // continues checking against the previous point
    if (point_prev == nullptr) {
      point_prev = &point;
      continue;
    }

    // checks that temperature is increasing
    if (point.temperature <= point_prev->temperature) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid tension point, temperature is constant"
                              " or decreasing";
        messages->push_back(message);
      }
    }

    // checks that tension is constant or decreasing
    if (point_prev->tension_horizontal < point.tension_horizontal) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid tension point, tension is increasing.";
        messages->push_back(message);
      }
    }

    // adjusts the previous pointer
    point_prev = &point;
  }

  // validates weight-unit
  if ((weight_unit <= 0) || (100 < weight_unit)) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid unit weight";
      messages->push_back(message);
    }
  }

  // returns validation status
  return is_valid;
}

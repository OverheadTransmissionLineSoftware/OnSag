// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "sag_span.h"

SagSpan::SagSpan() {
  description = "";
  notes = "";
  temperature_base = -999999;
  temperature_interval = -999999;
}

SagSpan::~SagSpan() {
}

bool SagSpan::Validate(const bool& is_included_warnings,
                       std::list<ErrorMessage>* messages) const {
  // initializes
  bool is_valid = true;
  ErrorMessage message;
  message.title = "SAG SPAN";

  // validates cable
  if (cable.Validate(is_included_warnings, messages) == false) {
    is_valid = false;
  }

  // validates method
  if (method.Validate(is_included_warnings, messages) == false) {
    is_valid = false;
  }

  // validates structure-ahead
  if (structure_ahead.Validate(is_included_warnings, messages) == false) {
    is_valid = false;
  }

  // validates structure-back
  if (structure_back.Validate(is_included_warnings, messages) == false) {
    is_valid = false;
  }

  // validates temperature-base
  if (temperature_base < 0) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid base temperature";
      messages->push_back(message);
    }
  }

  // validates temperature-interval
  if (temperature_interval <= 0) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid interval temperature";
      messages->push_back(message);
    }
  }

  // returns validation status
  return is_valid;
}

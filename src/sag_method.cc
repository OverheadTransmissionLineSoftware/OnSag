// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_method.h"

SagMethod::SagMethod() {
  end = SagMethod::SpanEndType::kNull;
  type = SagMethod::Type::kNull;
  wave_return = -9999;
}

SagMethod::~SagMethod() {
}

bool SagMethod::Validate(const bool& /**is_included_warnings**/,
                         std::list<ErrorMessage>* messages) const {
  // initializes
  bool is_valid = true;
  ErrorMessage message;
  message.title = "SAG METHOD";

  // validates based on type
  if (type == SagMethod::Type::kDynamometer) {
    if (end == SagMethod::SpanEndType::kNull) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid span end type";
        messages->push_back(message);
      }
    }
  } else if (type == SagMethod::Type::kStopWatch) {
    if (wave_return <= 0) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid return wave";
        messages->push_back(message);
      }
    }
  } else if (type == SagMethod::Type::kTransit) {
    if ((point_transit.x == -999999) || (point_transit.y == -999999)) {
      is_valid = false;
      if (messages != nullptr) {
        message.description = "Invalid transit position";
        messages->push_back(message);
      }
    }
  } else {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid sag method type";
      messages->push_back(message);
    }
  }

  // returns validation status
  return is_valid;
}

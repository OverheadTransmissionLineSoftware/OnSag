// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "onsag/sag_structure.h"

SagStructure::SagStructure() {
  point_attachment = Point2d<double>();
}

SagStructure::~SagStructure() {
}

bool SagStructure::Validate(const bool& /**is_included_warnings**/,
                            std::list<ErrorMessage>* messages) const {
  // initializes
  bool is_valid = true;
  ErrorMessage message;
  message.title = "SAG STRUCTURE";

  // validates position-attachment
  if ((point_attachment.x == -999999) || (point_attachment.y == -999999)) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid attachment point";
      messages->push_back(message);
    }
  }

  // returns validation status
  return is_valid;
}

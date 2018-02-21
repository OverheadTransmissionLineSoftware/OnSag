// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_ONSAG_ONSAGCONFIG_H_
#define OTLS_ONSAG_ONSAGCONFIG_H_

#include <string>

#include "models/base/units.h"
#include "wx/cmndata.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This struct stores the application configuration settings.
struct OnSagConfig {
 public:
  /// \var color_background
  ///   The background color.
  wxColour color_background;

  /// \var is_maximized_frame
  ///   An indicator that determines if the frame is maximized when created.
  ///   This is only updated on startup/exit.
  bool is_maximized_frame;

  /// \var level_log
  ///   The log level of the application.
  wxLogLevelValues level_log;

  /// \var perspective
  ///   The AUI perspective, which is based on the user layout of panes. The
  ///   perspective is loaded/saved on application startup/exit, and modified
  ///   on view open/close.
  std::string perspective;

  /// \var size_frame
  ///   The main application frame size.
  wxSize size_frame;

  /// \var units
  ///   The measurement unit system.
  units::UnitSystem units;
};

#endif  // OTLS_ONSAG_ONSAGCONFIG_H_

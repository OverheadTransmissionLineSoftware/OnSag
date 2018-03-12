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
/// This class has the options for the ProfilePlotPane.
struct ProfilePlotOptions {
 public:
  /// \var color_catenary
  ///   The catenary color.
  wxColour color_catenary;

  /// \var color_method
  ///   The sagging method color.
  wxColour color_method;

  /// \var scale_horizontal
  ///   The horizontal scale factor.
  int scale_horizontal;

  /// \var scale_vertical
  ///   The vertical scale factor.
  int scale_vertical;

  /// \var thickness_line
  ///   The line thickness.
  int thickness_line;
};

/// \par OVERVIEW
///
/// This struct stores the application configuration settings.
struct OnSagConfig {
 public:
  /// \var color_background
  ///   The background color.
  wxColour color_background;

  /// \var data_page
  ///   The page setup data for printing.
  wxPageSetupDialogData* data_page;

  /// \var is_maximized_frame
  ///   An indicator that determines if the frame is maximized when created.
  ///   This is only updated on startup/exit.
  bool is_maximized_frame;

  /// \var level_log
  ///   The log level of the application.
  wxLogLevelValues level_log;

  /// \var options_plot_profile
  ///   The profile plot pane options.
  ProfilePlotOptions options_plot_profile;

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

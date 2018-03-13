// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef ONSAG_FILE_HANDLER_H_
#define ONSAG_FILE_HANDLER_H_

#include <string>

#include "onsag/on_sag_config.h"

/// \par OVERVIEW
///
/// This class handles reading and writing application files that are not
/// document related.
class FileHandler {
 public:
  /// \brief Loads the config file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[out] config
  ///   The application configuration settings.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadConfigFile(const std::string& filepath,
                            OnSagConfig& config);

  /// \brief Saves the application configuration file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] config
  ///   The application configuration settings.
  static void SaveConfigFile(const std::string& filepath,
                             const OnSagConfig& config);
};

#endif  // ONSAG_FILE_HANDLER_H_

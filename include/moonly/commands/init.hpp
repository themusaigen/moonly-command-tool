#pragma once

#include <argparse/argparse.hpp>

namespace moonly {

/// @brief Command to initialize a new project with default configuration.
class initialize_command {
public:
  /// @brief Processes the "init" command.
  ///
  /// @param command The argument parser instance.
  static void process(argparse::ArgumentParser& command) noexcept;
};

} // namespace moonly

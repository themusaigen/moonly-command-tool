#pragma once

#include <argparse/argparse.hpp>

namespace moonly {
/// @brief Command to bundle the project into a single Lua file.
class bundle_command {
public:
  /// @brief Processes the "bundle" command.
  ///
  /// @param command The argument parser instance.
  static void
      process([[maybe_unused]] argparse::ArgumentParser& command) noexcept;
};

} // namespace moonly

#pragma once

#include <argparse/argparse.hpp>

namespace moonly {
/// @brief Command to pack project files into a ZIP archive.
class pack_command {
public:
  /// @brief Processes the "pack" command.
  ///
  /// @param command The argument parser instance.
  static void
      process([[maybe_unused]] argparse::ArgumentParser& command) noexcept;
};
} // namespace moonly

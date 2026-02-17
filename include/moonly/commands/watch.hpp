#pragma once

#include <argparse/argparse.hpp>

namespace moonly {
class watch_command {
public:
  static void process(argparse::ArgumentParser& parser) noexcept;
};
} // namespace moonly

#pragma once

#include <string>
#include <format>
#include <iostream>

namespace moonly {
class console {
public:
  static void output(const std::string& text) {
    std::cout << text;
  }

  template<typename... Args>
  static void output(const std::string& format, const Args&... args) {
    std::cout << std::vformat(format, std::make_format_args(args...));
  }
};
} // namespace moonly

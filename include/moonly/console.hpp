#pragma once

#include <string>
#include <format>
#include <iostream>

namespace moonly {
class console {
private:
  static bool m_disable_output;

public:
  static void disable() {
    m_disable_output = true;
  }

  static void enable() {
    m_disable_output = false;
  }

  static void output(const std::string& text) {
    if (m_disable_output) {
      return;
    }

    std::cout << text;
  }

  template<typename... Args>
  static void output(const std::string& format, const Args&... args) {
    if (m_disable_output) {
      return;
    }

    std::cout << std::vformat(format, std::make_format_args(args...));
  }
};
} // namespace moonly

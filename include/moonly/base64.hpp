#pragma once

#include <string>
#include <span>
#include <cstdint>

namespace moonly {
class base64 {
public:
  [[nodiscard]] static auto encode(std::span<std::uint8_t> bytes) noexcept
      -> std::string;
};
} // namespace moonly

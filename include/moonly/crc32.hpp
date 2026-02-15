#pragma once

#include <vector>
#include <cstdint>

namespace moonly {
class crc32 {
public:
  static void init() noexcept;

  [[nodiscard]] static auto get(std::uint8_t  byte,
                                std::uint32_t crc = 0xFFFFFFFFUL) noexcept
      -> std::uint32_t;
  [[nodiscard]] static auto get(const std::vector<std::uint8_t>& data) noexcept
      -> std::uint32_t;
};
} // namespace moonly

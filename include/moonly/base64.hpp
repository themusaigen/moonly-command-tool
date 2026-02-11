#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace moonly {
class base64 {
public:
  [[nodiscard]] static auto encode(const std::vector<std::uint8_t>& data,
                                   std::size_t size) noexcept -> std::string;
};
} // namespace moonly

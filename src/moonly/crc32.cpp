#include <moonly/crc32.hpp>

#include <array>

using namespace moonly;

static std::array<std::uint32_t, 256> crc32_table;

// NOLINTBEGIN(*-magic-numbers, *-array-index)
void crc32::init() noexcept {
  constexpr auto kPolynom = 0xEDB88320U;

  for (std::uint32_t i = 0; i < 256; ++i) {
    std::uint32_t crc = i;
    for (int k = 0; k < 8; ++k) {
      if ((crc & 1) != 0U) {
        crc = kPolynom ^ (crc >> 1);
      } else {
        crc = crc >> 1;
      }
    }
    crc32_table[i] = crc;
  }
}

auto crc32::get(std::uint8_t byte, std::uint32_t crc) noexcept
    -> std::uint32_t {
  return crc32_table[(crc ^ byte) & 0xFFU] ^ (crc >> 8);
}

auto crc32::get(const std::vector<std::uint8_t>& data) noexcept
    -> std::uint32_t {
  std::uint32_t crc{0xFFFFFFFFUL};

  for (const auto& byte : data) {
    crc = get(byte, crc);
  }

  return crc ^ 0xFFFFFFFFU;
}
// NOLINTEND(*-magic-numbers, *-array-index)

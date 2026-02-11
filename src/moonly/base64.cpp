#include <moonly/base64.hpp>

#include <array>

using namespace moonly;

static const std::array<char, 65> b64_table = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                               "abcdefghijklmnopqrstuvwxyz"
                                               "0123456789+/"};

auto base64::encode(const std::vector<std::uint8_t>& data,
                    std::size_t size) noexcept -> std::string {
  constexpr auto k1B64Byte  = 6;
  constexpr auto k2B64Bytes = 12;
  constexpr auto k3B64Bytes = 18;
  constexpr auto k1Byte     = 8;
  constexpr auto k2Bytes    = 16;
  constexpr auto kB64Mask   = 0x3F;

  std::string result;
  result.reserve((size + 2) / 3 * 4);
  std::size_t bits{0};

  // NOLINTBEGIN(*-array-index)
  for (; bits + 3 < size; bits += 3) {
    uint32_t triple = (static_cast<uint32_t>(data[bits]) << k2Bytes) |
                      (static_cast<uint32_t>(data[bits + 1]) << k1Byte) |
                      static_cast<uint32_t>(data[bits + 2]);

    result.push_back(b64_table[(triple >> k3B64Bytes) & kB64Mask]);
    result.push_back(b64_table[(triple >> k2B64Bytes) & kB64Mask]);
    result.push_back(b64_table[(triple >> k1B64Byte) & kB64Mask]);
    result.push_back(b64_table[triple & kB64Mask]);
  }

  if (bits < size) {
    uint32_t triple = static_cast<uint32_t>(data[bits]) << k2Bytes;
    result.push_back(b64_table[(triple >> k3B64Bytes) & kB64Mask]);

    if (bits + 1 < size) {
      triple |= uint32_t(data[bits + 1]) << k1Byte;
      result.push_back(b64_table[(triple >> k2B64Bytes) & kB64Mask]);
      result.push_back(b64_table[(triple >> k1B64Byte) & kB64Mask]);
    } else {
      result.push_back(b64_table[(triple >> k2B64Bytes) & kB64Mask]);
      result.push_back('=');
    }

    if (bits + 2 < size) {
      result.push_back(b64_table[triple & kB64Mask]);
    } else {
      result.push_back('=');
    }
  }
  // NOLINTEND(*-array-index)

  return result;
}

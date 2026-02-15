#include <moonly/filereader.hpp>
#include <moonly/base64.hpp>
#include <moonly/crc32.hpp>

#include <fstream>

using namespace moonly;

auto filereader::size(std::size_t& size) noexcept -> filereader& {
  m_size = &size;
  return *this;
}

auto filereader::text(std::string& text) noexcept -> filereader& {
  m_text = &text;
  m_mode &= ~std::ios::binary;
  return *this;
}

auto filereader::binary(std::vector<std::uint8_t>& binary) noexcept
    -> filereader& {
  m_binary = &binary;
  m_mode |= std::ios::binary;
  return *this;
}

auto filereader::strip_bundle(bool strip) noexcept -> filereader& {
  m_strip_bundle = strip;
  return *this;
}

auto filereader::crc32(std::uint32_t& crc32) noexcept -> filereader& {
  m_crc32 = &crc32;
  return *this;
}

auto filereader::base64(std::vector<std::string>& chunks,
                        std::size_t chunksize) noexcept -> filereader& {
  m_base64_chunks    = &chunks;
  m_base64_chunksize = chunksize;
  return *this;
}

// NOLINTBEGIN(*-cognitive-complexity, *-reinterpret-cast)
auto filereader::read() noexcept -> bool {
  constexpr auto kRawChunkSize = 65536ULL;
  constexpr auto kU32Max       = 0xFFFFFFFFU;

  std::ifstream file(m_path, m_mode);
  if (!file) {
    return false;
  }

  // Determine the physical size of the file.
  if (m_size != nullptr) {
    file.seekg(0, std::ios::end);
    *m_size = static_cast<std::uint64_t>(file.tellg());
    file.seekg(0, std::ios::beg);
  }

  // Start reading.
  std::uint32_t crc{kU32Max};
  bool          in_bundle_section{false};

  // Reading script now, ignore crc, etc...
  if (m_text != nullptr && m_strip_bundle) {
    std::string line;
    while (std::getline(file, line)) {
      if (line == "-- moonly bundle start") {
        in_bundle_section = true;
        continue;
      }
      if (line == "-- moonly bundle stop") {
        in_bundle_section = false;
        continue;
      }
      if (!in_bundle_section) {
        *m_text += line + '\n';
      }
    }

    return file.eof();
  }

  // Create buffer.
  std::vector<char> buffer(kRawChunkSize);
  while (file) {
    // Read the file.
    file.read(buffer.data(), kRawChunkSize);

    // No bytes read, break.
    const auto bytes_read = static_cast<std::uint64_t>(file.gcount());
    if (bytes_read == 0) {
      break;
    }

    if (m_crc32 != nullptr) {
      for (std::size_t i = 0; i < bytes_read; i++) {
        crc = moonly::crc32::get(static_cast<std::uint8_t>(buffer[i]), crc);
      }
    }

    if (m_text != nullptr) {
      m_text->append(buffer.data(), bytes_read);
    } else if (m_binary != nullptr) {
      m_binary->insert(
          m_binary->end(),
          buffer.begin(),
          buffer.begin() +
              static_cast<std::vector<std::uint8_t>::difference_type>(
                  bytes_read));
    }

    if (m_base64_chunks != nullptr) {
      auto base64 = moonly::base64::encode(
          {reinterpret_cast<std::uint8_t*>(buffer.data()), bytes_read});
      for (size_t i = 0; i < base64.size(); i += m_base64_chunksize) {
        m_base64_chunks->emplace_back(base64.substr(i, m_base64_chunksize));
      }
    }
  }

  if (m_crc32 != nullptr) {
    *m_crc32 = crc ^ kU32Max;
  }

  return file.eof();
}
// NOLINTEND(*-cognitive-complexity, *-reinterpret-cast)

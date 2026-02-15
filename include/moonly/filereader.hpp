#pragma once

#include <filesystem>
#include <ios>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace moonly {
class filereader {
private:
  static constexpr auto kBase64ChunkSize = 2048;

  std::filesystem::path      m_path;
  std::ios::openmode         m_mode;
  std::size_t*               m_size{nullptr};
  std::uint32_t*             m_crc32{nullptr};
  std::string*               m_text{nullptr};
  std::vector<std::uint8_t>* m_binary{nullptr};
  std::vector<std::string>*  m_base64_chunks{nullptr};
  std::size_t                m_base64_chunksize{0};
  bool                       m_strip_bundle{false};

public:
  explicit filereader(std::filesystem::path path,
                      std::ios::openmode    mode = std::ios::binary)
      : m_path(std::move(path))
      , m_mode(mode) {}

  auto size(std::size_t& size) noexcept -> filereader&;
  auto text(std::string& text) noexcept -> filereader&;
  auto binary(std::vector<std::uint8_t>& binary) noexcept -> filereader&;
  auto crc32(std::uint32_t& crc32) noexcept -> filereader&;
  auto base64(std::vector<std::string>& chunks,
              std::size_t chunksize = kBase64ChunkSize) noexcept -> filereader&;
  auto strip_bundle(bool strip = true) noexcept -> filereader&;

  [[nodiscard]] auto read() noexcept -> bool;
};
} // namespace moonly

#include <moonly/utility.hpp>
#include <moonly/base64.hpp>

#include <algorithm>
#include <array>
#include <fstream>

namespace fs = std::filesystem;

namespace moonly {
auto utility::read_file(const fs::path& path) noexcept -> std::string {
  if (!fs::exists(path)) {
    return {};
  }

  std::ifstream file{path};
  if (!file.is_open()) {
    return {};
  }

  std::string line;
  std::string text;
  bool        skip{false};
  while (std::getline(file, line)) {
    if (line == "-- moonly bundle start") {
      if (skip) {
        skip = false;
        continue;
      }
    }

    if (skip) {
      continue;
    }

    if (line == "-- moonly bundle stop") {
      skip = true;
    } else {
      text += line += '\n';
    }
  }

  return text;
}

auto utility::convert_backslashes(std::string str) noexcept -> std::string {
  size_t pos{};
  while ((pos = str.find('/')) != std::string::npos) {
    str.replace(pos, 1, "\\");
  }
  return str;
};

auto utility::read_binary_as_base64(const fs::path& path) noexcept
    -> std::vector<std::string> {
  constexpr auto kRawChunkSize    = 65536ULL;
  constexpr auto kBase64ChunkSize = 2048;

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return {};
  }

  std::vector<std::string>  chunks;
  std::vector<std::uint8_t> raw(kRawChunkSize);

  const auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  while (file.tellg() < size) {
    size_t bytes_to_read = std::min<decltype(kRawChunkSize)>(
        kRawChunkSize,
        static_cast<size_t>(size) - static_cast<size_t>(file.tellg()));
    // NOLINTBEGIN(*-reinterpret-cast)
    file.read(reinterpret_cast<char*>(raw.data()),
              static_cast<std::streamsize>(bytes_to_read));
    // NOLINTEND(*-reinterpret-cast)
    if (!file) {
      break;
    }

    std::string base64 = base64::encode(raw, bytes_to_read);
    for (size_t i = 0; i < base64.size(); i += kBase64ChunkSize) {
      chunks.push_back(base64.substr(i, kBase64ChunkSize));
    }
  }

  return chunks;
}

auto utility::read_file_as_binary(const fs::path& path) noexcept
    -> std::vector<std::vector<std::uint8_t>> {
  constexpr auto kChunkSize = 64ULL * 1024ULL; // 64Kbyte

  if (!fs::exists(path)) {
    return {};
  }

  std::ifstream file{path, std::ios::binary | std::ios::ate};
  if (!file.is_open()) {
    return {};
  }

  const auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<std::uint8_t>              buffer(size);
  std::vector<std::vector<std::uint8_t>> chunks;

  while (file.tellg() < size) {
    size_t bytes_to_read = std::min<decltype(kChunkSize)>(
        kChunkSize,
        static_cast<size_t>(size) - static_cast<size_t>(file.tellg()));
    file.read(reinterpret_cast<char*>(buffer.data()), bytes_to_read);
    if (!file) {
      break;
    }

    chunks.emplace_back(buffer.begin(), buffer.begin() + file.gcount());
  }

  return chunks;
}

auto utility::remove_root_directory(const fs::path& path) noexcept -> fs::path {
  auto current = path;
  while (current.has_parent_path()) {
    current = current.parent_path();
  }
  return fs::relative(path, current);
}

auto utility::split(std::string str, const std::string& delimiter)
    -> std::vector<std::string> {
  std::vector<std::string> result;

  size_t pos{};
  while ((pos = str.find(delimiter)) != std::string::npos) {
    result.push_back(str.substr(0, pos));
    str.erase(0, pos + delimiter.size());
  }
  return result;
}

auto utility::concat(const std::vector<std::string>& tokens,
                     const std::string&              delimiter) -> std::string {
  std::string result;
  for (size_t i = 0; i < tokens.size(); ++i) {
    result += tokens[i];
    if (i != tokens.size() - 1) {
      result += delimiter;
    }
  }
  return result;
}

auto utility::is_text_file(const fs::path& path) noexcept -> bool {
  constexpr std::array text_extensions = {".txt",
                                          ".csv",
                                          ".json",
                                          ".xml",
                                          ".yaml",
                                          ".yml",
                                          ".ini",
                                          ".log",
                                          ".lua",
                                          ".js",
                                          ".html",
                                          ".css",
                                          ".sh",
                                          ".bat",
                                          ".cmd",
                                          ".cfg",
                                          ".conf",
                                          ".md"};

  const auto ext = path.extension().string();
  return std::ranges::any_of(text_extensions, [&](const auto& text_ext) {
    return ext == text_ext;
  });
}

} // namespace moonly

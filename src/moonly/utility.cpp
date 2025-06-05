#include <moonly/utility.hpp>

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

  return {std::istreambuf_iterator<char>(file), {}};
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
    size_t bytes_to_read =
        std::min(kChunkSize,
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

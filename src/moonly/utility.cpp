#include <moonly/utility.hpp>

#include <algorithm>
#include <unordered_set>

namespace fs = std::filesystem;

namespace moonly {
auto utility::convert_backslashes(std::string str) noexcept -> std::string {
  size_t pos{};
  while ((pos = str.find('/')) != std::string::npos) {
    str.replace(pos, 1, "\\");
  }
  return str;
};

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
  static const std::unordered_set<std::string> extensions = {
      ".txt", ".csv", ".json", ".xml",  ".yaml", ".yml",       ".ini",
      ".log", ".lua", ".js",   ".html", ".css",  ".sh",        ".bat",
      ".cmd", ".cfg", ".conf", ".md",   ".toml", ".properties"};

  auto ext = path.extension().string();
  std::ranges::transform(ext, ext.begin(), ::tolower);
  return extensions.contains(ext);
}

} // namespace moonly

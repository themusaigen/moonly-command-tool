#include <moonly/configuration.hpp>

#include <utility>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace moonly {

configuration::configuration(nlohmann::json object)
    : m_object(std::move(object)) {}

auto configuration::project_name() const noexcept -> std::string {
  return m_object.contains("name") ? m_object["name"] : "my-project";
}

auto configuration::source_directory_name() const noexcept -> std::string {
  return m_object.contains("source") ? m_object["source"] : "src";
}

auto configuration::core_script_path() const noexcept -> std::string {
  return source_directory_name() + "\\init.lua";
}

auto configuration::library_directory_name() const noexcept -> std::string {
  return m_object.contains("library") ? m_object["library"] : "lib";
}

auto configuration::distribute_directory_name() const noexcept -> std::string {
  if (!m_object.contains("distribute")) {
    return "dist";
  }

  if (!m_object["distribute"].contains("output")) {
    return "dist";
  }

  return m_object["distribute"]["output"];
}

auto configuration::distribute_additional_directories() const noexcept
    -> std::vector<std::string> {
  if (!m_object.contains("distribute")) {
    return {};
  }

  if (!m_object["distribute"].contains("additionalDirs")) {
    return {};
  }

  return m_object["distribute"]["additionalDirs"];
}

auto configuration::distribute_ignored_directories() const noexcept
    -> std::vector<std::string> {
  if (!m_object.contains("distribute")) {
    return {};
  }

  if (!m_object["distribute"].contains("ignoredDirs")) {
    return {};
  }

  return m_object["distribute"]["ignoredDirs"];
}

auto configuration::distribute_additional_files() const noexcept
    -> std::vector<std::string> {
  if (!m_object.contains("distribute")) {
    return {};
  }

  if (!m_object["distribute"].contains("additionalFiles")) {
    return {};
  }

  return m_object["distribute"]["additionalFiles"];
}

auto configuration::distribute_constants() const noexcept
    -> std::unordered_map<std::string, nlohmann::json> {
  if (!m_object.contains("distribute")) {
    return {};
  }

  if (!m_object["distribute"].contains("constants")) {
    return {};
  }

  return m_object["distribute"]["constants"];
}

auto configuration::is_path_ignored(
    const std::filesystem::path& path) const noexcept -> bool {
  auto convert_backslashes = [](std::string str) {
    size_t pos{};
    while ((pos = str.find('/')) != std::string::npos) {
      str.replace(pos, 1, "\\");
    }
    return str;
  };

  return std::ranges::any_of(distribute_ignored_directories(),
                             [&path, &convert_backslashes](const auto& dir) {
                               return path.string().contains(
                                   convert_backslashes(dir));
                             });
}

auto configuration::get() noexcept -> configuration {
  if (!fs::exists("project.json")) {
    return {};
  }

  std::fstream file("project.json");
  if (!file.is_open()) {
    return {};
  }

  nlohmann::json json;
  file >> json;
  return {json};
}

} // namespace moonly

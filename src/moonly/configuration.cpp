#include <moonly/configuration.hpp>
#include <moonly/utility.hpp>
#include <glob.hpp>

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

auto configuration::include_patterns() const noexcept
    -> std::vector<std::string> {
  if (!m_object.contains("distribute")) {
    return {};
  }

  if (!m_object["distribute"].contains("include")) {
    return {};
  }

  return m_object["distribute"]["include"];
}

auto configuration::exclude_patterns() const noexcept
    -> std::vector<std::string> {
  if (!m_object.contains("distribute")) {
    return {};
  }

  if (!m_object["distribute"].contains("exclude")) {
    return {};
  }

  return m_object["distribute"]["exclude"];
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
  return std::ranges::any_of(exclude_patterns(), [&path](const auto& pattern) {
    return glob::matches(path, utility::convert_backslashes(pattern));
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

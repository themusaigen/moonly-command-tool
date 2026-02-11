#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace moonly {

/// @brief Class representing the project configuration loaded from JSON.
class configuration {
public:
  using json_type = nlohmann::json;

  /// @brief Default constructor creates an empty configuration.
  configuration() = default;

  /// @brief Constructs a configuration from a JSON object.
  ///
  /// @param object The JSON object to initialize the configuration with.
  configuration(json_type object);

  /// @brief Gets the project name.
  ///
  /// @return Project name or "my-project" if not specified.
  [[nodiscard]] auto project_name() const noexcept -> std::string;

  /// @brief Gets the source directory name.
  ///
  /// @return Source directory name or "src" if not specified.
  [[nodiscard]] auto source_directory_name() const noexcept -> std::string;

  /// @brief Gets the path to the core script file.
  ///
  /// @return Path to "init.lua" inside the source directory.
  [[nodiscard]] auto core_script_path() const noexcept -> std::string;

  /// @brief Gets the library directory name.
  ///
  /// @return Library directory name or "lib" if not specified.
  [[nodiscard]] auto library_directory_name() const noexcept -> std::string;

  /// @brief Gets the distribute output directory name.
  ///
  /// @return Output directory name or "dist" if not specified.
  [[nodiscard]] auto distribute_directory_name() const noexcept -> std::string;

  /// @brief Gets constants to inject during distribution.
  ///
  /// @return Map of constant names to values.
  [[nodiscard]] auto distribute_constants() const noexcept
      -> std::unordered_map<std::string, json_type>;

  /// @brief Checks whether a given path is marked as ignored in the
  /// configuration.
  ///
  /// @param path The path to check.
  /// @return True if the path is ignored, false otherwise.
  [[nodiscard]] auto is_path_ignored(const fs::path& path) const noexcept
      -> bool;

  [[nodiscard]] auto include_patterns() const noexcept
      -> std::vector<std::string>;

  [[nodiscard]] auto exclude_patterns() const noexcept
      -> std::vector<std::string>;

  /// @brief Loads and parses the configuration from "project.json".
  ///
  /// @return A new configuration instance based on the file content.
  [[nodiscard]] static auto get() noexcept -> configuration;

private:
  json_type m_object;
};

} // namespace moonly

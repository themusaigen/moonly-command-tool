#pragma once

#include <format>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace moonly {

/// @brief Class responsible for bundling Lua scripts and resources into a
/// single file.
class bundler {
private:
  std::stringstream m_data;
  std::size_t       m_indent{0};
  std::size_t       m_indent_backup{0};

public:
  /// @brief Adds a standard header to the output.
  void add_header() noexcept;

  /// @brief Adds a standard fodder to the output.
  void add_fodder() noexcept;

  /// @brief Performs constant propagation from configuration.
  void constant_propagation() noexcept;

  /// @brief Adds all scripts in a directory.
  ///
  /// @param directory The source directory.
  void add_scripts(const fs::path& directory) noexcept;

  /// @brief Adds a single script to the bundle.
  ///
  /// @param file Path to the Lua file.
  void add_script(const fs::path& file) noexcept;

  /// @brief Adds core script (e.g., init.lua).
  ///
  /// @param path Path to the core script.
  void add_core_file(const fs::path& path) noexcept;

  /// @brief Adds resource files (text or binary).
  ///
  /// @param directory Directory containing resources.
  void add_resources(const fs::path& directory) noexcept;

  /// @brief Adds a specific resource to the bundle.
  ///
  /// @param resource Path to the resource.
  void add_resource(const fs::path& resource) noexcept;

  /// @brief Adds a text file to the bundle as a Lua string.
  ///
  /// @param file Path to the text file.
  void add_text_file(const fs::path& file) noexcept;

  /// @brief Adds a binary file to the bundle as a byte array.
  ///
  /// @param file Path to the binary file.
  void add_binary(const fs::path& file) noexcept;

  /// @brief Returns the final bundled content.
  [[nodiscard]] auto data() const noexcept -> std::string;

private:
  void store_indent() noexcept;
  void load_indent() noexcept;
  void indent(std::size_t level = 1) noexcept;
  void unindent(std::size_t level = 1) noexcept;

  void print_file(const std::string& data);
  void print_file_writing(const std::string& content);
  void new_line() noexcept;

  void print_no_indent(const std::string& text) {
    m_data << text;
  }

  template<typename... Args>
  void print_no_indent(const std::string& format, const Args&... args) {
    m_data << std::vformat(format, std::make_format_args(args...));
  }

  void print(const std::string& text) {
    m_data << std::string(m_indent, ' ') << text;
  }

  template<typename... Args>
  void print(const std::string& format, const Args&... args) {
    m_data << std::string(m_indent, ' ')
           << std::vformat(format, std::make_format_args(args...));
  }
};
} // namespace moonly

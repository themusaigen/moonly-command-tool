#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace moonly {

/// @brief Utility class for common file and string operations.
class utility {
public:
  /// @brief Reads the contents of a text file.
  ///
  /// @param path The path to the file.
  /// @return The content as a std::string, or empty if the file could not be
  /// read.
  [[nodiscard]] static auto read_file(const fs::path& path) noexcept
      -> std::string;

  /// @brief Reads the contents of a binary file into a vector of bytes.
  ///
  /// @param path The path to the file.
  /// @return A vector of uint8_t containing the file data, or empty on failure.
  [[nodiscard]] static auto read_file_as_binary(const fs::path& path) noexcept
      -> std::vector<std::vector<std::uint8_t>>;

  [[nodiscard]] static auto read_binary_as_base64(const fs::path& path) noexcept
      -> std::vector<std::string>;

  /// @brief Removes the root directory from a given path.
  ///
  /// @param path The full path.
  /// @return A new path without the root directory.
  [[nodiscard]] static auto remove_root_directory(const fs::path& path) noexcept
      -> fs::path;

  /// @brief Splits a string by a delimiter.
  ///
  /// @param str The input string.
  /// @param delimiter The delimiter to split on.
  /// @return A vector of substrings.
  [[nodiscard]] static auto split(std::string str, const std::string& delimiter)
      -> std::vector<std::string>;

  [[nodiscard]] static auto convert_backslashes(std::string str) noexcept
      -> std::string;

  /// @brief Joins a vector of strings using a delimiter.
  ///
  /// @param tokens The vector of strings.
  /// @param delimiter The delimiter to use between elements.
  /// @return The concatenated string.
  [[nodiscard]] static auto concat(const std::vector<std::string>& tokens,
                                   const std::string& delimiter) -> std::string;

  /// @brief Checks if a file is likely a text file based on its extension.
  ///
  /// @param path The path to the file.
  /// @return True if the file is considered a text file; false otherwise.
  [[nodiscard]] static auto is_text_file(const fs::path& path) noexcept -> bool;
};
} // namespace moonly

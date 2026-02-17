#include <moonly/commands/watch.hpp>
#include <moonly/commands/bundle.hpp>
#include <moonly/commands/pack.hpp>
#include <moonly/configuration.hpp>
#include <moonly/console.hpp>

#include <glob.hpp>

#include <Windows.h>

#include <algorithm>
#include <thread>
#include <chrono>
#include <format>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <optional>

using namespace moonly;

// NOLINTBEGIN(*-cognitive-complexity)
void watch_command::process(argparse::ArgumentParser& parser) noexcept {
  namespace fs = std::filesystem;

  auto project = configuration::get();

  const auto mode = parser.get<std::string>("-m");
  const auto delay =
      std::max<std::uint64_t>(parser.get<std::uint64_t>("-d"), 50);

  std::unordered_map<fs::path, FILETIME> resources;

  auto get_file_modify_time =
      [](const fs::path& path) -> std::optional<_FILETIME> {
    auto* handle = CreateFileA(path.generic_string().c_str(),
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);

    if (handle == INVALID_HANDLE_VALUE) {
      return std::nullopt;
    }

    FILETIME time;
    auto     result = GetFileTime(handle, nullptr, nullptr, &time);
    CloseHandle(handle);

    if (result == 0) {
      return std::nullopt;
    }

    return time;
  };

  auto collect = [&](const std::string&              pattern,
                     std::optional<std::string_view> corefile) -> void {
    for (const auto& path : glob::rglob(pattern)) {
      if (fs::is_directory(path)) {
        continue;
      }

      if (resources.contains(path.generic_string())) {
        continue;
      }

      if (!(corefile.has_value() && path.string() == corefile.value())) {
        if (project.is_path_ignored(path)) {
          continue;
        }
      }

      auto modify_time = get_file_modify_time(path);
      if (modify_time.has_value()) {
        resources[path.generic_string()] = modify_time.value();
      }
    }
  };

  auto collect_resources = [&]() -> void {
    collect(std::format("{}/**", project.source_directory_name()),
            project.core_script_path());

    for (const auto& pattern : project.include_patterns()) {
      collect(pattern, std::nullopt);
    }
  };

  collect_resources();

  while (true) {
    bool needs_to_update{false};
    for (auto& [path, modify_time] : resources) {
      auto new_modify_time = get_file_modify_time(path);
      if (!new_modify_time.has_value()) {
        continue;
      }

      if ((new_modify_time->dwLowDateTime != modify_time.dwLowDateTime) ||
          (new_modify_time->dwHighDateTime != modify_time.dwHighDateTime)) {
        needs_to_update = true;

        modify_time = new_modify_time.value();

        console::output("-> Detected changes in {}. Re{}...\n",
                        path.generic_string(),
                        mode == "bundle" ? "bundling" : "packing");
      }
    }

    if (needs_to_update) {
      console::disable();

      if (mode == "bundle") {
        bundle_command::process();
      } else {
        pack_command::process();
      }

      console::enable();

      // Recollecting new files if needed.
      collect_resources();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }
}
// NOLINTEND(*-cognitive-complexity)

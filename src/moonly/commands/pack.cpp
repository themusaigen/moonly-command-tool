#include <moonly/commands/pack.hpp>
#include <moonly/configuration.hpp>
#include <moonly/console.hpp>
#include <moonly/utility.hpp>
#include <glob.hpp>

#include <nlohmann/json.hpp>
#include <zip.h>

#include <format>
#include <filesystem>

namespace fs = std::filesystem;

namespace moonly {

void pack_command::process() noexcept {
  using namespace nlohmann;

  auto project = configuration::get();

  const auto source_dir = project.source_directory_name();
  if (!fs::exists(source_dir)) {
    console::output("Source directory '{}' does not exist.\n", source_dir);
    return;
  }

  const auto output_dir = project.distribute_directory_name();
  if (!fs::exists(output_dir)) {
    if (!fs::create_directories(output_dir)) {
      console::output("Failed to create output directory '{}'.\n", output_dir);
      return;
    }
  }

  const auto name     = project.project_name();
  const auto zip_path = std::format("{}/{}.zip", output_dir, name);

  console::output("--> Creating ZIP file at '{}'.\n", zip_path);

  zip_t* zip = zip_open(zip_path.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  if (zip == nullptr) {
    console::output("Failed to create ZIP file '{}'.\n", zip_path);
    return;
  }

  console::output("--> Opened '{}' ZIP file.\n", zip_path);

  // Add core init.lua as first entry
  const auto core_script_path = source_dir + "/init.lua";
  if (fs::exists(core_script_path)) {
    const auto entry_name = std::format("{}-init.lua", name);
    zip_entry_open(zip, entry_name.c_str());
    zip_entry_fwrite(zip, core_script_path.c_str());
    zip_entry_close(zip);

    console::output("--> Packed '{}'.\n", entry_name);
  } else {
    console::output("Core file 'init.lua' not found in source directory.\n");
  }

  // Add all other Lua scripts from source directory
  for (const auto& entry : fs::recursive_directory_iterator{source_dir}) {
    // clang-format off
    if (entry.is_directory() ||
        entry.path() == core_script_path ||
        project.is_path_ignored(entry.path())) {
      continue;
    }
    // clang-format on

    const auto rel_path = utility::remove_root_directory(entry.path());
    const auto ext      = entry.path().extension().string();

    if (ext == ".lua") {
      console::output(" --> Packed Lua script: {}.\n", entry.path().string());
    } else {
      console::output(" --> Packed file: {}.\n", entry.path().string());
    }

    zip_entry_open(zip, rel_path.string().c_str());
    zip_entry_fwrite(zip, entry.path().string().c_str());
    zip_entry_close(zip);
  }

  console::output("--> Collecting resources.\n");

  std::vector<fs::path> resources;

  // Globing files that matches user's include patterns.
  for (const auto& path : glob::rglob(project.include_patterns())) {
    if (!fs::is_directory(path) && !project.is_path_ignored(path)) {
      resources.push_back(path);
    }
  }

  // Add resource.
  for (const auto& resource : resources) {
    console::output(" --> Packed resource: {}.\n", resource.string());

    zip_entry_open(zip, resource.string().c_str());
    zip_entry_fwrite(zip, resource.string().c_str());
    zip_entry_close(zip);
  }

  zip_close(zip);
  console::output("--> Successfully created ZIP archive at '{}'.\n", zip_path);
}
} // namespace moonly

#include <moonly/commands/bundle.hpp>
#include <moonly/configuration.hpp>
#include <moonly/bundler.hpp>
#include <moonly/utility.hpp>
#include <moonly/console.hpp>
#include <glob.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace moonly {
void bundle_command::process(
    [[maybe_unused]] argparse::ArgumentParser& command) noexcept {
  auto project = configuration::get();

  // Nothing to bundle.
  auto source_dir = project.source_directory_name();
  if (!fs::exists(source_dir)) {
    return;
  }

  auto output_dir = project.distribute_directory_name();
  if (!fs::exists(output_dir)) {
    fs::create_directories(output_dir);
  }

  auto output_path = [&output_dir, name = project.project_name()]() {
    return std::format("{}/{}.lua", output_dir, name);
  }();

  std::ofstream file(output_path);
  if (!file.is_open()) {
    return;
  }

  bundler bundler;
  bundler.add_header();

  // Preparing for collecting resources.
  std::vector<fs::path> resources;

  console::output("-> Collecting resources...\n");

  // Globing files that matches user's include patterns.
  for (const auto& path : glob::rglob(project.include_patterns())) {
    if (!fs::is_directory(path)) {
      resources.push_back(path);
    }
  }

  // Removing files that matches exclude patterns.
  for (const auto& pattern : project.exclude_patterns()) {
    std::erase_if(resources, [&pattern](const auto& resource) {
      return glob::matches(resource, utility::convert_backslashes(pattern));
    });
  }

  // Checking is we need to add kernel functions like b64decode, crc32 and
  // etc...
  bool needs_to_add_kernel{false};
  for (const auto& resource : resources) {
    if (!resource.has_extension() || !utility::is_text_file(resource)) {
      needs_to_add_kernel = true;
      break;
    }
  }

  // Add if needed.
  if (needs_to_add_kernel) {
    bundler.add_kernel_functions();
  }

  // Add resources firstly.
  for (const auto& resource : resources) {
    bundler.add_resource(resource);
  }

  // Add cleanup fodder.
  if (needs_to_add_kernel) {
    bundler.add_fodder();
  }

  // Perform constant propagation.
  bundler.constant_propagation();

  // Add scripts.
  bundler.add_scripts(source_dir);

  // Add core file.
  bundler.add_core_file(project.core_script_path());

  file << bundler.data();
  file.close();
}
} // namespace moonly

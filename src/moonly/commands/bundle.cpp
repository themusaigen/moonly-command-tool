#include <moonly/commands/bundle.hpp>
#include <moonly/configuration.hpp>
#include <moonly/bundler.hpp>

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
  bundler.constant_propagation();
  bundler.add_scripts(source_dir, project.core_script_path());

  for (const auto& dir : project.distribute_additional_directories()) {
    bundler.add_resources(dir);
  }

  for (const auto& file : project.distribute_additional_files()) {
    bundler.add_resource(file);
  }

  bundler.add_core_file(project.core_script_path());

  file << bundler.data();
  file.close();
}
} // namespace moonly

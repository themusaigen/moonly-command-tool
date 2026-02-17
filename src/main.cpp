#include <moonly/commands/watch.hpp>
#include <moonly/commands/bundle.hpp>
#include <moonly/commands/init.hpp>
#include <moonly/commands/pack.hpp>
#include <moonly/crc32.hpp>

#include <argparse/argparse.hpp>

using namespace argparse;

auto main(int argc, char** argv) -> int {
  moonly::crc32::init();

  ArgumentParser program("moonly", "3.0.0");

  // 'init' command.
  ArgumentParser init_command("init");
  init_command.add_description("Initializes new moonly project.");
  init_command.add_argument("-n", "--name")
      .help("Name of new project.")
      .default_value("my-project");
  init_command.add_argument("-s", "--src")
      .help("Source directory of new project.")
      .default_value("src");
  init_command.add_argument("-l", "--lib")
      .help("Dependencies directory of new project.")
      .default_value("lib");
  init_command.add_argument("-d", "--dist")
      .help("Distribute directory of new project.")
      .default_value("dist");
  init_command.add_argument("-e", "--editor")
      .help("Specify code-editor. Moonly will generate settings for it.")
      .choices("vscode");
  init_command.add_argument("--dirs").help("Auto create directories.").flag();

  // 'pack' command.
  ArgumentParser pack_command("pack");
  pack_command.add_description("Packs all project into one ZIP archive.");

  // 'Bundle' command.
  ArgumentParser bundle_command("bundle");
  bundle_command.add_description("Bundles all project into one Lua file.");

  // 'Watch' command.
  constexpr auto kDefaultWatchDelay = 1000ULL;

  ArgumentParser watch_command("watch");
  watch_command.add_description("Watches the changes of your project directory "
                                "and repacks or rebundles it.");
  watch_command.add_argument("-m", "--mode")
      .help("Mode (repack/rebundle).")
      .choices("pack", "bundle")
      .default_value("bundle");
  watch_command.add_argument("-d", "--delay")
      .help("Delay between each check.")
      .scan<'u', std::uint64_t>()
      .default_value(kDefaultWatchDelay);

  // Add commands.
  program.add_subparser(init_command);
  program.add_subparser(pack_command);
  program.add_subparser(bundle_command);
  program.add_subparser(watch_command);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    std::cerr << program;
    return 1;
  }

  if (program.is_subcommand_used(init_command)) {
    moonly::initialize_command::process(init_command);
  } else if (program.is_subcommand_used(pack_command)) {
    moonly::pack_command::process();
  } else if (program.is_subcommand_used(bundle_command)) {
    moonly::bundle_command::process();
  } else if (program.is_subcommand_used(watch_command)) {
    moonly::watch_command::process(watch_command);
  }
}

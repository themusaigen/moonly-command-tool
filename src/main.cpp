#include <moonly/commands/bundle.hpp>
#include <moonly/commands/init.hpp>
#include <moonly/commands/pack.hpp>

#include <argparse/argparse.hpp>

using namespace argparse;

extern void process_init_command(ArgumentParser& parser);
extern void process_pack_command(ArgumentParser& parser);
extern void process_bundle_command(ArgumentParser& parser);

auto main(int argc, char** argv) -> int {
  ArgumentParser program("moonly", "2.1.0");

  // 'init' command.
  ArgumentParser init_command("init");
  init_command.add_description("Initializes new moonly project.");
  init_command.add_argument("-n", "--name")
      .help("Name of new project.")
      .default_value("my-project");
  init_command.add_argument("-s", "--src")
      .help("Source directory of new project")
      .default_value("src");
  init_command.add_argument("-l", "--lib")
      .help("Dependencies directory of new project")
      .default_value("lib");
  init_command.add_argument("-d", "--dist")
      .help("Distribute directory of new project")
      .default_value("dist");
  init_command.add_argument("-e", "--editor")
      .help("Specify code-editor. Moonly will generate settings for it. ")
      .choices("vscode");
  init_command.add_argument("--dirs").help("Auto create directories").flag();

  // 'pack' command.
  ArgumentParser pack_command("pack");
  pack_command.add_description("Packs all project into one ZIP archive.");

  // 'Bundle' command.
  ArgumentParser bundle_command("bundle");
  bundle_command.add_description("Bundles all project into one Lua file.");

  // Add commands.
  program.add_subparser(init_command);
  program.add_subparser(pack_command);
  program.add_subparser(bundle_command);

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
    moonly::pack_command::process(pack_command);
  } else if (program.is_subcommand_used(bundle_command)) {
    moonly::bundle_command::process(bundle_command);
  }
}

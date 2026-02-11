#include <moonly/commands/init.hpp>
#include <moonly/console.hpp>

#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <format>

namespace fs = std::filesystem;

namespace moonly {

void initialize_command::process(argparse::ArgumentParser& command) noexcept {
  using json = nlohmann::json;

  // Default values if not provided
  const auto name    = command.get<std::string>("-n");
  const auto source  = command.get<std::string>("-s");
  const auto library = command.get<std::string>("-l");
  const auto dist    = command.get<std::string>("-d");

  // Build JSON configuration
  json project{
      {"name",       name                                                    },
      {"source",     source                                                  },
      {"library",    library                                                 },
      {"distribute",
       {{"output", dist},
        {"include", json::array()},
        {"exclude", json::array()},
        {"constants",
         {{"MOONLY_BUNDLED", true}, {"MOONLY_BUNDLE_TIMESTAMP", "<$date>"}}}}}
  };

  console::output("--> Initialized '{}' project.\n", name);
  console::output(" --> Source directory:  {}.\n", source);
  console::output(" --> Library directory: {}.\n", library);
  console::output(" --> Distribute directory: {}.\n", dist);

  if (command.is_used("-e")) {
    console::output(" --> Initialized configuration for '{}'.\n",
                    command.get<std::string>("-e"));
  }

  if (command.is_used("--dirs")) {
    console::output(
        " --> Created '{}' and '{}' directories.\n", source, library);
  }

  // Write project.json
  std::ofstream file("project.json");
  if (file.is_open()) {
    file << std::setw(2) << project << '\n';
    file.close();
  } else {
    console::output(" --> Failed to create 'project.json'.\n");
    return;
  }

  // Generate .vscode settings if requested
  if (command.is_used("-e")) {
    const auto editor = command.get<std::string>("-e");
    if (editor == "vscode") {
      fs::create_directories(".vscode");

      json settings{
          {"[lua]",
           {{"editor.defaultFormatter", "sumneko.lua"},
            {"files.encoding", "windows1251"}}                               },
          {"Lua.runtime.version",     "LuaJIT"                               },
          {"Lua.runtime.path",
           {std::format("{}/?.lua", source),
            std::format("{}/?/init.lua", source),
            std::format("{}/?.lua", library),
            std::format("{}/?/init.lua", library)}                           },
          {"Lua.diagnostics.globals",
           {"main",
            "onExitScript",
            "onQuitGame",
            "onScriptLoad",
            "onScriptTerminate",
            "onScriptMessage",
            "onSystemMessage",
            "onReceivePacket",
            "onReceiveRpc",
            "onSendPacket",
            "onSendRpc",
            "onWindowMessage",
            "onStartNewGame",
            "onLoadGame",
            "onSaveGame"}                                                    },
          {"Lua.workspace.library",   {"${addons}/moonloader/module/library"}}
      };

      std::ofstream vscode_file(".vscode/settings.json");
      if (vscode_file.is_open()) {
        vscode_file << std::setw(2) << settings << '\n';
        vscode_file.close();
      } else {
        console::output(" --> Failed to create '.vscode/settings.json'.\n");
      }
    }
  }

  // Create directories if requested
  if (command.is_used("--dirs")) {
    fs::create_directories(source);
    fs::create_directories(library);
  }
}

} // namespace moonly

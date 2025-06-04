#include <moonly/bundler.hpp>
#include <moonly/configuration.hpp>
#include <moonly/utility.hpp>
#include <moonly/console.hpp>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

namespace moonly {

void bundler::add_header() noexcept {
  print("-- Bundled using Moonly CLI\n");
  print("-- Get Moonly CLI from "
        "<github.com/themusaigen/moonly-command-tool>\n");
  print("-- Get Moonly from <github.com/themusaigen/moonly>\n\n");

  console::output("-> Added file header.\n");
}

void bundler::constant_propagation() noexcept {
  using namespace std::chrono;

  console::output("-> Performing constant propagation.\n");

  auto constants = configuration::get().distribute_constants();
  for (const auto& [key, value] : constants) {
    console::output(" --> Added '{}' constant.\n", key);

    if (!value.is_null() && !value.is_boolean() && !value.is_number() &&
        !value.is_string()) {
      continue;
    }

    auto data = nlohmann::to_string(value);
    if (value.is_string()) {
      if (value.template get<std::string>() == "<$date>") {
        data = std::to_string(
            duration_cast<milliseconds>(system_clock::now().time_since_epoch())
                .count());
      }
    }

    print("{} = {}\n", key, data);
  }

  if (!constants.empty()) {
    new_line();
  }
}

void bundler::add_scripts(const fs::path&                directory,
                          const std::optional<fs::path>& ignored) noexcept {
  if (!fs::exists(directory)) {
    return;
  }

  console::output("-> Collecting scripts from '{}' directory.\n",
                  directory.string());

  auto project = configuration::get();

  for (const auto& entry : fs::recursive_directory_iterator{directory}) {
    if (entry.is_directory() || project.is_path_ignored(entry)) {
      continue;
    }

    if (entry.path().extension() != ".lua") {
      continue;
    }

    if (entry.path() == ignored) {
      continue;
    }

    add_script(entry.path());
  }
}

void bundler::add_script(const std::filesystem::path& file) noexcept {
  auto without_root_directory = utility::remove_root_directory(file);
  auto directories = utility::split(without_root_directory.string(), "\\");

  auto package = std::string{};
  if (file.filename() == "init.lua") {
    package = utility::concat(directories, ".");
  } else {
    if (!directories.empty()) {
      package = utility::concat(directories, ".") + ".";
    }

    package += file.filename().replace_extension("").string();
  }

  console::output(
      " -> Bundled '{}' script as package '{}'.\n", file.string(), package);

  print("-- Package <{}> ({})", file.filename().string(), file.string());
  new_line();

  print("package.preload[\"{}\"] = function()", package);
  new_line();
  indent(2);
  print_file(utility::read_file(file));
  unindent(2);
  print("end");
  new_line();
  new_line();
}

void bundler::add_core_file(const fs::path& path) noexcept {
  print("-- Core file <init.lua> ({})", path.string());
  new_line();

  print(utility::read_file(path));
  new_line();

  console::output("-> Bundled core 'init.lua'.\n");
}

void bundler::add_resources(const std::filesystem::path& directory) noexcept {
  if (!fs::exists(directory)) {
    return;
  }

  console::output("-> Collecting resources from {} directory.\n",
                  directory.string());

  auto project = configuration::get();

  for (const auto& entry : fs::recursive_directory_iterator{directory}) {
    if (entry.is_directory() || project.is_path_ignored(entry)) {
      continue;
    }

    add_resource(entry.path());
  }
}

void bundler::add_resource(const std::filesystem::path& resource) noexcept {
  if (!fs::exists(resource)) {
    return;
  }

  auto is_text_file = utility::is_text_file(resource);
  if (is_text_file) {
    if (resource.extension() == ".lua") {
      add_script(resource);
    } else {
      add_text_file(resource);
    }
  } else {
    add_binary(resource);
  }
}

void bundler::add_text_file(const std::filesystem::path& file) noexcept {
  auto text = utility::read_file(file);
  if (text.empty()) {
    return;
  }

  const fs::path path = "moonloader" / file;

  console::output(" -> Bundled text file '{}'.\n", file.string());

  print("-- Text File <{}> ({})", file.filename().string(), file.string());
  new_line();

  print("if not doesFileExist(\"{}\") then", path.generic_string());
  new_line();
  indent(2);

  print(R"(local file = io.open("{}", "w+"))", path.generic_string());
  new_line();
  print("if not file then");
  new_line();
  indent(2);
  print("error(\"can't open the file\")");
  new_line();
  unindent(2);
  print("end");
  new_line();

  print("file:write([[");
  print_no_indent(text);
  print_no_indent("]])");

  new_line();
  print("file:close()");
  new_line();
  unindent(2);
  print("end");
  new_line();
  new_line();
}

void bundler::add_binary(const std::filesystem::path& file) noexcept {
  auto binary = utility::read_file_as_binary(file);
  if (binary.empty()) {
    return;
  }

  const fs::path path = "moonloader" / file;

  console::output(" -> Bundled binary file '{}'.\n", file.string());

  print("-- Binary File <{}> ({})", file.filename().string(), file.string());
  new_line();

  print("if not doesFileExist(\"{}\") then", path.generic_string());
  new_line();
  indent(2);

  print("local binary_data = {");
  new_line();
  indent(2);

  constexpr auto kBytesInRow = 8;
  for (std::size_t i = 0; i < binary.size(); ++i) {
    if (i % kBytesInRow == 0 && i > 0) {
      new_line();
    }

    if (i % kBytesInRow == kBytesInRow) {
      print_no_indent("0x{:X}, ", static_cast<int>(binary[i]));
    } else {
      print("0x{:X}, ", static_cast<int>(binary[i]));
    }
  }

  unindent(2);
  new_line();
  print("}");
  new_line();

  print(R"(local file = io.open("{}", "w+b"))", path.generic_string());
  new_line();
  print("if not file then");
  new_line();
  indent(2);
  print("error(\"can't open the file\")");
  new_line();
  unindent(2);
  print("end");
  new_line();
  new_line();

  print("local buffer = \"\"");
  new_line();
  print("for _, byte in ipairs(binary_data) do");
  new_line();
  indent(2);
  print("buffer = buffer .. string.char(byte)");
  new_line();
  unindent(2);
  print("end");
  new_line();
  new_line();

  print("file:write(buffer)");
  new_line();
  print("file:close()");
  new_line();
  unindent(2);
  print("end");
  new_line();
  new_line();
}

auto bundler::data() const noexcept -> std::string {
  return m_data.str();
}

void bundler::print_file(const std::string& data) {
  for (const auto& line : utility::split(data, "\n")) {
    print(line);
    new_line();
  }
}

void bundler::new_line() noexcept {
  m_data << "\n";
}

void bundler::unindent(std::size_t level) noexcept {
  if (level > m_indent) {
    m_indent = 0;
  } else {
    m_indent -= level;
  }
}

void bundler::indent(std::size_t level) noexcept {
  load_indent();

  m_indent += level;
}

void bundler::load_indent() noexcept {
  if (m_indent_backup != 0) {
    m_indent        = m_indent_backup;
    m_indent_backup = 0;
  }
}

void bundler::store_indent() noexcept {
  m_indent_backup = m_indent;
  m_indent        = 0;
}
} // namespace moonly

#include <moonly/bundler.hpp>
#include <moonly/configuration.hpp>
#include <moonly/utility.hpp>
#include <moonly/console.hpp>
#include <moonly/filereader.hpp>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

namespace moonly {

void bundler::add_header() noexcept {
  std::string ascii_art =
      R"(-- +==================================================+ --
-- |                                                  | --
-- |                                                  | --
-- |                                                  | --
-- |    __  __                           _            | --
-- |   |  \/  |   ___     ___    _ __   | |  _   _    | --
-- |   | |\/| |  / _ \   / _ \  | '_ \  | | | | | |   | --
-- |   | |  | | | (_) | | (_) | | | | | | | | |_| |   | --
-- |   |_|  |_|  \___/   \___/  |_| |_| |_|  \__, |   | --
-- |                                         |___/    | --
-- |                                                  | --
-- |                                                  | --
-- |                                                  | --
-- +==================================================+ --)";

  print(ascii_art);
  new_line();

  // NOLINTBEGIN(*-magic-numbers)
  print("-- Version");
  indent(4);
  { print(": 3.0.0-preview-2\n"); }
  unindent(4);
  print("-- Moonly");
  indent(5);
  { print(": github.com/themusaigen/moonly\n"); }
  unindent(5);
  print("-- CLI");
  indent(8);
  { print(": github.com/themusaigen/moonly-command-tool\n\n"); }
  unindent(8);
  // NOLINTEND(*-magic-numbers)

  console::output("-> Added file header.\n");
}

void bundler::add_kernel_functions() noexcept {
  print("local _moonly_b64charset = (function()\n");
  indent(2);
  {
    print("local t = {}\n");
    print("for i = 0, 255 do t[i] = -1 end\n");
    print("local chars = "
          "\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
          "\"\n");
    print("for i = 1, #chars do t[string.byte(chars, i)] = i - 1 end\n");
    print("t[string.byte('=')] = -2\n");
    print("return t\n");
  }
  unindent(2);
  print("end)()\n\n");

  print("local _moonly_crc32table = (function()\n");
  indent(2);
  {
    print("local t = {}\n");
    print("for i = 0, 255 do\n");
    indent(2);
    {
      print("local c = i\n");
      print("for _ = 0, 7 do\n");
      indent(2);
      {
        print("if bit.band(c, 1) == 1 then\n");
        indent(2);
        { print("c = bit.bxor(0xEDB88320, bit.rshift(c, 1))\n"); }
        unindent(2);
        print("else\n");
        indent(2);
        { print("c = bit.rshift(c, 1)\n"); }
        unindent(2);
        print("end\n");
        print("c = bit.band(c, 0xFFFFFFFF)\n");
      }
      unindent(2);
      print("end\n");
      print("t[i] = c\n");
    }
    unindent(2);
    print("end\n");
    print("return t\n");
  }
  unindent(2);
  print("end)()\n\n");

  print("local function _moonly_b64decode(str)\n");
  indent(2);
  {
    print("local result = {}\n");
    print("local bits = 0\n");
    print("local buffer = 0\n\n");

    print("for i = 1, #str do\n");
    indent(2);
    {
      print("local c = _moonly_b64charset[str:byte(i)]\n");
      print("if c == -1 then\n");
      indent(2);
      { print("error(\"moonly: unknown base64 character\")\n"); }
      unindent(2);
      print("elseif c ~= -2 then\n");
      indent(2);
      {
        print("buffer = bit.bor(bit.lshift(buffer, 6), c)\n");
        print("bits = bits + 6\n\n");

        print("while bits >= 8 do\n");
        indent(2);
        {
          print("bits = bits - 8\n");
          print("result[#result + 1] = string.char(bit.band(bit.rshift(buffer, "
                "bits), 0xFF))\n");
        }
        unindent(2);
        print("end\n");
      }
      unindent(2);
      print("end\n");
    }
    unindent(2);
    print("end\n\n");

    print("return table.concat(result)\n");
  }
  unindent(2);
  print("end\n\n");

  print("local function _moonly_crc32(str)\n");
  indent(2);
  {
    print("local crc = 0xFFFFFFFF\n");
    print("for i = 1, #str do\n");
    indent(2);
    {
      print("local b = str:byte(i)\n");
      print("crc = bit.bxor(_moonly_crc32table[bit.band(bit.bxor(crc, b), "
            "0xFF)], bit.rshift(crc, 8))\n");
      print("crc = bit.band(crc, 0xFFFFFFFF)\n");
    }
    unindent(2);
    print("end\n");
    print("crc = bit.band(bit.bxor(crc, 0xFFFFFFFF), 0xFFFFFFFF)\n");
    print("return crc >= 0 and crc or crc + 4294967296\n");
  }
  unindent(2);
  print("end\n\n");
}

void bundler::add_fodder() noexcept {
  print("-- Cleanup moonly's kernel.\n");
  print("_moonly_crc32table = nil\n");
  print("_moonly_crc32 = nil\n");
  print("_moonly_b64charset = nil\n");
  print("_moonly_b64decode = nil\n");
  print("-- Cleanup ended.\n\n");

  console::output("-> Added file fodder.\n");
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
    } else if (value.is_null()) {
      data = "nil";
    }

    print("{} = {}\n", key, data);
  }

  if (!constants.empty()) {
    new_line();
  }
}

void bundler::add_scripts(const fs::path& directory) noexcept {
  if (!fs::exists(directory)) {
    return;
  }

  console::output("-> Collecting scripts from '{}' directory.\n",
                  directory.string());

  auto project     = configuration::get();
  auto core_script = project.core_script_path();

  for (const auto& entry : fs::recursive_directory_iterator{directory}) {
    // clang-format off
    if (entry.is_directory() ||
        entry.path() == core_script ||
        entry.path().extension() != ".lua" ||
        project.is_path_ignored(entry.path())) {
      continue;
    }
    // clang-format on

    add_script(entry.path());
  }
}

void bundler::add_script(const std::filesystem::path& file) noexcept {
  filereader reader(file);

  std::string content{};

  // clang-format off
  reader
    .text(content)
    .strip_bundle();
  // clang-format on

  if (!reader.read()) {
    return;
  }

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

  console::output(" -> Bundled '{}' script as package '{}'.\n",
                  file.generic_string(),
                  package);

  // NOLINTBEGIN(*-magic-numbers)
  print("-- Package");
  indent(4);
  { print(": {}\n", file.filename().string()); }
  unindent(4);
  print("-- Path");
  indent(7);
  { print(": {}\n", file.generic_string()); }
  unindent(7);
  // NOLINTEND(*-magic-numbers)

  print("do\n");
  indent(2);
  {
    print("package.preload[\"{}\"] = function()\n", package);
    indent(2);
    {
      for (const auto& line : utility::split(content, "\n")) {
        print(line);
        new_line();
      }
    }
    unindent(2);
    print("end\n");
  }
  unindent(2);
  print("end\n\n");
}

void bundler::add_core_file(const fs::path& path) noexcept {
  filereader reader(path);

  std::string content{};

  // clang-format off
  reader
    .text(content)
    .strip_bundle();
  // clang-format on

  if (!reader.read()) {
    return;
  }

  print("-- init.lua ({})\n", path.generic_string());
  print(content);

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
  filereader reader(file);

  std::string content{};
  std::size_t size{0};

  // clang-format off
  reader
    .size(size)
    .text(content);
  // clang-format on

  if (!reader.read()) {
    return;
  }

  console::output(" -> Bundled text file '{}'.\n", file.generic_string());

  print("-- Text");
  indent(4);
  { print(": {}\n", file.filename().string()); }
  unindent(4);
  print("-- Path");
  indent(4);
  { print(": {}\n", file.generic_string()); }
  unindent(4);
  print("-- Size");
  indent(4);
  { print(": {}\n", size); }
  unindent(4);

  print("do\n");
  indent(2);
  {
    bundle_directory_code(file);

    print("-- Creating a resource.\n");
    print("local expected_content = {}\n", escape_textfile(content));
    print("local expected_size = {}\n\n", size);

    const auto path = fs::path{"moonloader"} / file;

    print("local unpack = not doesFileExist(\"{}\")\n", path.generic_string());
    print("if not unpack then\n");
    indent(2);
    {
      print("local file = io.open(\"{}\", 'rb')\n", path.generic_string());
      print("local size = file:seek(\"end\")\n");
      print("file:seek(\"set\", 0)\n");
      print(
          R"(unpack = size ~= expected_size or file:read("*a"):gsub('\r\n', '\n'):gsub('\r', '\n') ~= expected_content:gsub('\r\n', '\n'):gsub('\r', '\n'))");
      new_line();
      print("file:close()\n");
    }
    unindent(2);
    print("end\n\n");

    print("if unpack then\n");
    indent(2);
    {
      print("local file = io.open(\"{}\", 'w+')\n", path.generic_string());
      print("if not file then\n");
      indent(2);
      {
        print("error(\"moonly: can't open file '{}'.\")\n",
              path.generic_string());
      }
      unindent(2);
      print("end\n\n");

      print("file:write(expected_content)\n");
      print("file:flush()\n");
      print("file:close()\n");
    }
    unindent(2);
    print("end\n");
  }
  unindent(2);
  print("end\n\n");
}

void bundler::add_binary(const std::filesystem::path& file) noexcept {
  filereader reader(file);

  std::vector<std::string> chunks{};
  std::size_t              size{};
  std::uint32_t            crc32{};

  // clang-format off
  reader
    .size(size)
    .crc32(crc32)
    .base64(chunks);
  // clang-format on

  if (!reader.read()) {
    return;
  }

  console::output(" -> Bundled binary file '{}'.\n", file.generic_string());

  // NOLINTBEGIN(*-magic-numbers)
  print("-- Binary");
  indent(4);
  { print(": {}\n", file.filename().string()); }
  unindent(4);
  print("-- Path");
  indent(6);
  { print(": {}\n", file.generic_string()); }
  unindent(6);
  print("-- Size");
  indent(6);
  { print(": {}\n", size); }
  unindent(6);
  print("-- CRC32");
  indent(5);
  { print(": {:X}\n", crc32); }
  unindent(5);
  // NOLINTEND(*-magic-numbers)

  print("do\n");
  indent(2);
  {
    bundle_directory_code(file);

    const auto path = fs::path{"moonloader"} / file;

    print("-- Creating a resource.\n");
    print("local expected_crc = {:#X}\n", crc32);
    print("local expected_size = {}\n\n", size);

    print("local unpack = not doesFileExist(\"{}\")\n", path.generic_string());
    print("if not unpack then\n");
    indent(2);
    {
      print(R"(local file = io.open("{}", "rb"))", path.generic_string());
      new_line();

      print("local filesize = file:seek(\"end\")\n");
      print("file:seek(\"set\", 0)\n");

      print(
          R"(unpack = expected_size ~= filesize or _moonly_crc32(file:read("*a")) ~= expected_crc)");
      new_line();
      print("file:close()\n");
    };
    unindent(2);
    print("end\n\n");

    print("if unpack then\n");
    indent(2);
    {
      print("local file = io.open(\"{}\", 'w+b')\n", path.generic_string());
      print("if not file then\n");
      indent(2);
      {
        print("error(\"moonly: can't open file '{}'.\")\n",
              path.generic_string());
      }
      unindent(2);
      print("end\n\n");

      print("local chunks = {\n");
      indent(2);
      {
        auto size{chunks.size()};
        for (std::size_t i = 0; i < size; i++) {
          print("\"{}\"", chunks[i]);

          if (i < size - 1) {
            print_no_indent(",\n");
          } else {
            print_no_indent("\n");
          }
        }
      }
      unindent(2);
      print("}\n\n");

      print("for _, chunk in ipairs(chunks) do\n");
      indent(2);
      { print("file:write(_moonly_b64decode(chunk))\n"); }
      unindent(2);
      print("end\n\n");

      print("file:flush()\n");
      print("file:close()\n");
    }
    unindent(2);
    print("end\n");
  }
  unindent(2);
  print("end\n\n");
}

auto bundler::data() const noexcept -> std::string {
  return m_data.str();
}

auto bundler::escape_textfile(const std::string& content) -> std::string {
  std::size_t length            = content.length();
  std::size_t long_string_level = 0;
  std::size_t index             = 0;

  while (index < length) {
    if (content[index] != ']') {
      index++;
      continue;
    }

    std::size_t new_long_string_level = 1;

    index++;

    while (index < length && content[index] == '=') {
      new_long_string_level++;
      index++;
    }

    if (index > length || content[index] != ']' ||
        new_long_string_level < long_string_level) {
      continue;
    }

    long_string_level = new_long_string_level;
  }

  auto long_string_level_str = std::string(long_string_level, '=');

  std::string trimmed = content;
  trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
  trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

  return std::format(
      "[{}[{}]{}]", long_string_level_str, trimmed, long_string_level_str);
}

void bundler::bundle_directory_code(const fs::path& resource) noexcept {
  auto directories = utility::split(resource.generic_string(), "/");
  if (!directories.empty()) {
    print("-- Creating resource directories, if necessary.\n");
    print("local directories = {\n");
    indent(2);
    {
      auto directory = fs::path{"moonloader"};
      auto size      = directories.size();
      for (std::size_t i = 0; i < size; i++) {
        directory /= directories[i];

        // Add directory to the list.
        print("\"{}\"", directory.generic_string());

        if (i < size - 1) {
          print_no_indent(",\n");
        } else {
          print_no_indent("\n");
        }
      }
    }
    unindent(2);
    print("}\n\n");

    print("for _, directory in ipairs(directories) do\n");
    indent(2);
    {
      print("if not doesDirectoryExist(directory) then\n");
      indent(2);
      { print("createDirectory(directory)\n"); }
      unindent(2);
      print("end\n");
    }
    unindent(2);
    print("end\n\n");
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
  m_indent += level;
}
} // namespace moonly

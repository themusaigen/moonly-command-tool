cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(
  moonly
  VERSION 3.1.0
  DESCRIPTION "Moonly CLI"
  LANGUAGES C CXX)

add_executable(${PROJECT_NAME})

add_subdirectory("third-party/argparse")
add_subdirectory("third-party/json")
add_subdirectory("third-party/zip")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
target_include_directories(${PROJECT_NAME} PUBLIC "third-party/glob")
target_link_libraries(${PROJECT_NAME} PRIVATE argparse
                                              nlohmann_json::nlohmann_json zip)

target_sources(
  ${PROJECT_NAME}
  PRIVATE "src/main.cpp"
          "src/moonly/configuration.cpp"
          "src/moonly/console.cpp"
          "src/moonly/commands/init.cpp"
          "src/moonly/commands/pack.cpp"
          "src/moonly/commands/bundle.cpp"
          "src/moonly/commands/watch.cpp"
          "src/moonly/bundler.cpp"
          "src/moonly/utility.cpp"
          "src/moonly/base64.cpp"
          "src/moonly/crc32.cpp"
          "src/moonly/filereader.cpp")

# Only if top project.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # .clangd configuration generator.
  add_subdirectory("scripts/clangd")
endif()

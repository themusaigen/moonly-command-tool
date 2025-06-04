set(CLANGD_PATH "${PROJECT_SOURCE_DIR}/.clangd")
set(CLANGD_MAIN_FLAGS
    "-Wall,
    -Wextra,
    -Wno-c++98-compat,
    -Wno-c++98-compat-pedantic,
    -Wno-unused-function,
    -Wno-unused-template")
set(CLANGD_M32 "-m32")

# Detecting architecture.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(CLANGD_FLAGS "${CLANGD_MAIN_FLAGS}")
else()
  set(CLANGD_FLAGS "${CLANGD_MAIN_FLAGS},
    ${CLANGD_M32}")
endif()

# Create the body of .clangd file.
set(CLANGD_BODY
    "CompileFlags:
  CompilationDatabase: build
  Add: [${CLANGD_FLAGS}]")

# Rewrite or create .clangd file.
file(WRITE ${CLANGD_PATH} ${CLANGD_BODY})

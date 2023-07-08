option(ENABLE_SPVREMAPPER "Enables building of SPVRemapper" ON)
option(ENABLE_GLSLANG_BINARIES "Builds glslangValidator and spirv-remap" OFF)
option(ENABLE_GLSLANG_JS "If using Emscripten, build glslang.js. Otherwise, builds a sample executable for binary-size testing." OFF)

option(ENABLE_RTTI "Enables RTTI" OFF)
option(ENABLE_EXCEPTIONS "Enables Exceptions" OFF)
option(ENABLE_OPT "Enables spirv-opt capability if present" ON)
option(ENABLE_HLSL "Enables HLSL" ON)

option(ENABLE_CTEST "Enables testing" OFF)
option(ENABLE_GLSLANG_INSTALL "Install" OFF)

add_subdirectory(glslang)

# target_include_directories(glslang PUBLIC ${ENGINE_ROOT_DIR}/3rd_party/Vulkan-Headers/include)

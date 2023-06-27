# PackageProject.cmake will be used to make our target installable
set(TARGET_NAME EngineRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

# source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# ---- Create library ----

# Note: for header-only libraries change all PUBLIC flags to INTERFACE and create an interface
# target: add_library(${TARGET_NAME} INTERFACE)
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# target_compile_definitions(${TARGET_NAME} PUBLIC JPH_DISABLE_CUSTOM_ALLOCATOR)

# Link dependencies
target_link_libraries(${TARGET_NAME} PUBLIC json11)
# target_link_libraries(${TARGET_NAME} PUBLIC fmt::fmt)
# target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog_header_only)
target_link_libraries(${TARGET_NAME} PUBLIC fmt::fmt-header-only)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC libzip::zip)

# target_link_libraries(${TARGET_NAME} PUBLIC soloud)
# target_link_libraries(${TARGET_NAME} PUBLIC imgui)
# target_link_libraries(${TARGET_NAME} PUBLIC Eigen3::Eigen)
# target_link_libraries(${TARGET_NAME} PUBLIC tinyobjloader::tinyobjloader)
# target_link_libraries(${TARGET_NAME} PUBLIC sol2)
# target_link_libraries(${TARGET_NAME} PUBLIC ${LUA_LIBRARIES})
# target_link_libraries(${TARGET_NAME} PUBLIC lz4::lz4)
# target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)
# target_link_libraries(${TARGET_NAME} PUBLIC Jolt)
# target_link_libraries(${TARGET_NAME} PUBLIC volk::volk_headers) # volk::volk
# target_link_libraries(${TARGET_NAME} PUBLIC unofficial::vulkan-memory-allocator::vulkan-memory-allocator)
# target_link_libraries(${TARGET_NAME} PUBLIC 
#   glslang::OSDependent 
#   glslang::glslang 
#   glslang::MachineIndependent 
#   glslang::GenericCodeGen 
#   glslang::OGLCompiler 
#   # glslang::glslangValidator 
#   # glslang::spirv-remap 
#   glslang::glslang-default-resource-limits 
#   glslang::SPVRemapper 
#   glslang::SPIRV 
#   glslang::HLSL
# )

target_link_directories(${TARGET_NAME} PUBLIC "$<IF:$<CONFIG:Debug>,${JSON11_LIB_DEBUG_DIRS},${JSON11_LIB_DIRS}>")
target_link_directories(${TARGET_NAME} PUBLIC "$<IF:$<CONFIG:Debug>,${JOLTPHYSICS_LIB_DEBUG_DIRS},${JOLTPHYSICS_LIB_DIRS}>")

target_include_directories(${TARGET_NAME} PUBLIC ${ENGINE_ROOT_DIR}/3rd_party/json11)
# target_include_directories(${TARGET_NAME} PUBLIC ${STB_INCLUDE_DIRS})
# target_include_directories(${TARGET_NAME} PUBLIC ${TINYGLTF_INCLUDE_DIRS})
# target_include_directories(${TARGET_NAME} PUBLIC ${LUA_INCLUDE_DIR})
# target_include_directories(${TARGET_NAME} PUBLIC ${JOLTPHYSICS_INCLUDE_DIRS})

# add_dependencies(${TARGET_NAME} ${SHADER_COMPILE_TARGET})

# target_include_directories(
#   ${TARGET_NAME}
#   PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/shader/generated/cpp>
# )

target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# target_include_directories(
#   ${TARGET_NAME}
#   PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime/function/render/include>
#   $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
# )

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match the project's regular header paths
string(TOLOWER ${TARGET_NAME}/version.h VERSION_HEADER_LOCATION)

# ## precompile
# set global vari used by precompile
set(ENGINE_RUNTIME_HEADS "${HEADER_FILES}" PARENT_SCOPE)

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
# cmake_policy(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(NAME ArchViz)

project(${NAME} VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include(CMakeDependentOption)

find_package(spdlog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
# find_package(volk CONFIG REQUIRED)
# find_package(libzip CONFIG REQUIRED)
# find_package(fmt CONFIG REQUIRED)
# find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)
# find_package(glfw3 CONFIG REQUIRED)

# find_package(Lua REQUIRED)

# find_package(LLVM CONFIG REQUIRED)
# find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
# find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
# find_package(tinyobjloader CONFIG REQUIRED)
# find_package(sol2 CONFIG REQUIRED)
# find_package(lz4 CONFIG REQUIRED)
# find_package(nlohmann_json CONFIG REQUIRED)
# find_package(Taskflow CONFIG REQUIRED)
# target_link_libraries(main PRIVATE Taskflow::Taskflow)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
        FATAL_ERROR
        "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif()

set(ARCHVIZ_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${ARCHVIZ_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

add_subdirectory(engine)

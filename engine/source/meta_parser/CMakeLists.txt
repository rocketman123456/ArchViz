# cmake_policy(SET CMP0074 OLD)
set(TARGET_NAME MetaParser)

find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)

# set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../CMake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
# include(HandleLLVMOptions)
# add_definitions(${LLVM_DEFINITIONS})
# statically link with Boost & LibClang
add_definitions(-DTIXML_USE_STL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_ROOT_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_ROOT_DIR}/bin)

# add LLVM includes
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/mustache)
include_directories(${ENGINE_ROOT_DIR}/3rd_party/mustache)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# header files are superflous, but some IDEs (Visual Studio) don't include
# them in the solution explorer without them being added to the list of sources
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

# set_target_properties(meta_parser PROPERTIES FOLDER "generator" )
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Tools")

target_include_directories(${TARGET_NAME} PRIVATE ${LLVM_INCLUDE_DIRS})

# link_directories(${LLVM_LIBRARY_DIR})
if(CMAKE_HOST_WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /O2 /Ob2")
    set(LLVM_clang_lib ${LLVM_LIBRARY_DIRS}/libclang.lib)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    set(LLVM_clang_lib ${LLVM_LIBRARY_DIRS}/libclang.so)
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    set(LLVM_clang_lib ${LLVM_LIBRARY_DIRS}/libclang.dylib)
else()
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    set(LLVM_clang_lib ${LLVM_LIBRARY_DIRS}/libclang.dylib)
endif()

target_link_libraries(${TARGET_NAME} ${LLVM_clang_lib})
# link_libraries(${LLVM_LIBRARY_DIRS})
# target_link_libraries(${TARGET_NAME} clang)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET_NAME}>" "${ENGINE_ROOT_DIR}/bin"
)

# copy resources on post build
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    # mustache templates directory
    COMMAND ${CMAKE_COMMAND} -E copy
    "${LLVM_clang_lib}"
    $<TARGET_FILE_DIR:${TARGET_NAME}>
)

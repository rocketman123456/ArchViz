set(ENGINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rd_party")

# set(ENGINE_ASSET_DIR "/asset")
# set(ENGINE_ASSET_TEST_DIR "/asset-test")

# set(DEPLOY_CONFIG_DIR "configs/deployment")
# set(DEVELOP_CONFIG_DIR "configs/development")

# if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#     add_compile_options("/MP")
#     set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ArchVizEditor)
# endif()

# set(glslangValidator_executable glslang::glslangValidator)

# if(EXISTS ${ENGINE_ROOT_DIR}/asset/)
# file(COPY "${ENGINE_ROOT_DIR}/asset/" DESTINATION "${CMAKE_BINARY_DIR}/asset")
# endif()

# if(EXISTS ${ENGINE_ROOT_DIR}/shader/glsl/)
# file(COPY "${ENGINE_ROOT_DIR}/shader/glsl/" DESTINATION "${CMAKE_BINARY_DIR}/shader/glsl/")
# endif()

# if(EXISTS ${ENGINE_ROOT_DIR}/shader/include/)
# file(COPY "${ENGINE_ROOT_DIR}/shader/include/" DESTINATION "${CMAKE_BINARY_DIR}/shader/include/")
# endif()

if(EXISTS ${ENGINE_ROOT_DIR}/config/)
    file(COPY "${ENGINE_ROOT_DIR}/config/" DESTINATION "${BINARY_ROOT_DIR}/config")
endif()

if(EXISTS ${ENGINE_ROOT_DIR}/template/)
    file(COPY "${ENGINE_ROOT_DIR}/template/" DESTINATION "${BINARY_ROOT_DIR}/template")
endif()

# set(SHADER_COMPILE_TARGET ArchVizShaderCompile)
# add_subdirectory(shader)

add_subdirectory(3rd_party)

add_subdirectory(source/meta_parser)
add_subdirectory(source/runtime)
# add_subdirectory(source/editor)
# add_subdirectory(source/playground)
add_subdirectory(source/unit_test)

set(CODEGEN_TARGET "ArchVizPreCompile")
include(${ARCHVIZ_ROOT_DIR}/cmake/precompile/precompile.cmake)
set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine")
add_dependencies("${CODEGEN_TARGET}" "MetaParser")

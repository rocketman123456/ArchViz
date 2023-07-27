set(ENGINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rd_party")
# set(ENGINE_ASSET_DIR "/asset")
# set(ENGINE_ASSET_TEST_DIR "/asset-test")
set(DEPLOY_CONFIG_DIR "config/deployment")
set(DEVELOP_CONFIG_DIR "config/development")

if(WIN32)
    add_compile_definitions("ARCHVIZ_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32")
elseif(UNIX)
    if(APPLE)
        add_compile_definitions("ARCHVIZ_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS")
        add_compile_definitions("ARCHVIZ_VK_ICD_FILENAMES=${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS/MoltenVK_icd.json")
    else()
        add_compile_definitions("ARCHVIZ_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Linux")
    endif()
else()
    message(FATAL_ERROR "Unknown Platform")
endif()

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
add_subdirectory(source/unit_test)
add_subdirectory(source/editor)
# add_subdirectory(source/playground)

if(PRECOMPILE_PROJECT)
    set(CODEGEN_TARGET "ArchVizPreCompile")
    include(${ARCHVIZ_ROOT_DIR}/cmake/precompile/precompile.cmake)
    set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine")
    add_dependencies("${CODEGEN_TARGET}" "MetaParser")
endif()

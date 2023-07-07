if(NOT TARGET soloud)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/soloud.cmake)
endif()

if(NOT TARGET eigen)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/eigen.cmake)
endif()

if(NOT TARGET json11)
    add_subdirectory(json11)
endif()

if(NOT TARGET imgui)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/imgui.cmake)
endif()

# if(NOT TARGET zlib)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/zlib.cmake)
# endif()

if(NOT TARGET libzip)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/libzip.cmake)
endif()

if(NOT TARGET glfw)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/glfw.cmake)
endif()

if(NOT TARGET tlsf)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/tlsf.cmake)
endif()

# if(NOT TARGET VulkanMemoryAllocator)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/vma.cmake)
# endif()

if(NOT TARGET glslang)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/glslang.cmake)
endif()

if(NOT TARGET spdlog)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/spdlog.cmake)
endif()

if(NOT TARGET fmt)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/fmt.cmake)
endif()

if(NOT TARGET Vulkan-Headers)
    add_subdirectory(Vulkan-Headers)
endif()

if(NOT TARGET SPIRV-Headers)
    add_subdirectory(SPIRV-Headers)
endif()

if(NOT TARGET volk)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/volk.cmake)
endif()

if(NOT TARGET mustache)
    add_subdirectory(mustache)
endif()

if(NOT TARGET tinygltf)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/tinygltf.cmake)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
endif()

# if(NOT TARGET x9)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/x9.cmake)
# endif()

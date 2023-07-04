# if(NOT TARGET soloud)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/soloud.cmake)
# endif()

if(NOT TARGET json11)
    add_subdirectory(json11)
endif()

# if(NOT TARGET libzip)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/libzip.cmake)
# endif()

if(NOT TARGET glfw)
    include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/glfw.cmake)
endif()

if(NOT TARGET vulkan_headers)
    add_subdirectory(Vulkan-Headers)
endif()

if(NOT TARGET volk)
    add_subdirectory(volk)
    target_include_directories(volk PUBLIC ${ENGINE_ROOT_DIR}/3rd_party/Vulkan-Headers/include)
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

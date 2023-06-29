# if(NOT TARGET soloud)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/soloud.cmake)
# endif()

if(NOT TARGET json11)
    add_subdirectory(json11)
endif()

# if(NOT TARGET libzip)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/libzip.cmake)
# endif()

# if(NOT TARGET glfw)
#     # add_subdirectory(glfw)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/glfw.cmake)
# endif()

if(NOT TARGET volk)
    add_subdirectory(volk)
endif()

if(NOT TARGET mustache)
    add_subdirectory(mustache)
endif()

if(NOT TARGET tinygltf)
    add_subdirectory(tinygltf)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
endif()

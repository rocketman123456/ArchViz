# if(NOT TARGET soloud)
#     include(${ARCHVIZ_ROOT_DIR}/cmake/3rd_party/soloud.cmake)
# endif()

if(NOT TARGET json11)
    add_subdirectory(json11)
endif()

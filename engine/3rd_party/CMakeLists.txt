# if(NOT TARGET soloud)
#     include(soloud.cmake)
# endif()

if(NOT TARGET soloud)
    add_subdirectory(json11)
endif()

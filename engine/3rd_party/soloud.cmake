file(GLOB_RECURSE soloud_audio_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/soloud/src/audiosource/*.cpp")
file(GLOB_RECURSE soloud_c_api_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/soloud/src/c_api/*.cpp")
file(GLOB_RECURSE soloud_core_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/soloud/src/core/*.cpp")
file(GLOB_RECURSE soloud_filter_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/soloud/src/filter/*.cpp")
file(GLOB_RECURSE soloud_tools_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/soloud/src/tools/*.cpp")
file(GLOB soloud_backend_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/soloud/src/backend/miniaudio/*.cpp")
add_library(soloud STATIC
    ${soloud_audio_sources}
    ${soloud_c_api_sources}
    ${soloud_core_sources}
    ${soloud_filter_sources}
    # ${soloud_tools_sources}
    ${soloud_backend_sources}
)
target_compile_definitions(soloud PRIVATE WITH_MINIAUDIO)
target_include_directories(soloud PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/soloud/include)

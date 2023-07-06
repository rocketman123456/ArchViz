file(GLOB_RECURSE tlsf_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/tlsf/*.c")

add_library(tlsf STATIC ${tlsf_sources})

target_include_directories(tlsf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tlsf)

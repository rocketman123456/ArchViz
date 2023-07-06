file(GLOB_RECURSE x9_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/x9/*.c")

add_library(x9 STATIC ${x9_sources})

target_compile_definitions(x9 PRIVATE X9_DEBUG)
target_include_directories(x9 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/x9)

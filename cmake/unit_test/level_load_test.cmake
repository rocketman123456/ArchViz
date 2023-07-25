add_executable(level_load_test level_load_test.cpp)

set_target_properties(level_load_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "level_load_test")
# set_target_properties(level_load_test PROPERTIES FOLDER "Engine")
# set_target_properties(level_load_test PROPERTIES CMAKE_CURRENT_BINARY_DIR "${BINARY_ROOT_DIR}")

target_include_directories(level_load_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(level_load_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(level_load_test PUBLIC EngineRuntime)
# target_compile_definitions(level_load_test PUBLIC UNIT_TEST)

set(POST_LEVEL_LOAD_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:level_load_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET level_load_test ${POST_LEVEL_LOAD_TEST_COMMANDS})
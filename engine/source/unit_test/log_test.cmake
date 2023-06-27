add_executable(log_test log_test.cpp)

set_target_properties(log_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "log_test")
set_target_properties(log_test PROPERTIES FOLDER "Engine")

target_include_directories(log_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(log_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(log_test PUBLIC EngineRuntime)

set(POST_LOG_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:log_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET log_test ${POST_LOG_TEST_COMMANDS})
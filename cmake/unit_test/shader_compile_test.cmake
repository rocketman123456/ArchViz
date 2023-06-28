add_executable(shader_compile_test shader_compile_test.cpp)

set_target_properties(shader_compile_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "shader_compile_test")
# set_target_properties(vfs_test PROPERTIES FOLDER "Engine")

target_include_directories(shader_compile_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(shader_compile_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(shader_compile_test PUBLIC EngineRuntime)
# target_compile_definitions(vfs_test PUBLIC UNIT_TEST)

set(POST_SHADER_COMPILE_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:shader_compile_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET shader_compile_test ${POST_SHADER_COMPILE_TEST_COMMANDS})
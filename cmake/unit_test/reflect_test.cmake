add_executable(reflect_test reflect_test.cpp)

set_target_properties(reflect_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "reflect_test")
# set_target_properties(reflect_test PROPERTIES FOLDER "Engine")

target_include_directories(reflect_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(reflect_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(reflect_test PUBLIC EngineRuntime)
# target_compile_definitions(reflect_test PUBLIC UNIT_TEST)

set(POST_REFLECT_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:reflect_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET reflect_test ${POST_REFLECT_TEST_COMMANDS})
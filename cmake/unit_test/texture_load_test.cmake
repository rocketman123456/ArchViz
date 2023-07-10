add_executable(texture_load_test texture_load_test.cpp)

set_target_properties(texture_load_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "texture_load_test")
# set_target_properties(texture_load_test PROPERTIES FOLDER "Engine")

target_include_directories(texture_load_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(texture_load_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(texture_load_test PUBLIC EngineRuntime)
# target_compile_definitions(texture_load_test PUBLIC UNIT_TEST)

set(POST_TEXTURE_LOAD_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:texture_load_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET texture_load_test ${POST_TEXTURE_LOAD_TEST_COMMANDS})
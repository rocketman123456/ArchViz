add_executable(resource_test resource_test.cpp)

set_target_properties(resource_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "resource_test")
# set_target_properties(resource_test PROPERTIES FOLDER "Engine")

target_include_directories(resource_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(resource_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(resource_test PUBLIC EngineRuntime)
# target_compile_definitions(resource_test PUBLIC UNIT_TEST)

set(POST_RESOURCE_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:resource_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET resource_test ${POST_RESOURCE_TEST_COMMANDS})
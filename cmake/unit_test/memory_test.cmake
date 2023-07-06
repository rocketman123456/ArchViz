add_executable(memory_test memory_test.cpp)

set_target_properties(memory_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "memory_test")
# set_target_properties(memory_test PROPERTIES FOLDER "Engine")

target_include_directories(memory_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(memory_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(memory_test PUBLIC EngineRuntime)
# target_compile_definitions(memory_test PUBLIC UNIT_TEST)

set(POST_MEMORY_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:memory_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET memory_test ${POST_MEMORY_TEST_COMMANDS})

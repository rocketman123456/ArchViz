add_executable(frame_graph_test frame_graph_test.cpp)

set_target_properties(frame_graph_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "frame_graph_test")
# set_target_properties(frame_graph_test PROPERTIES FOLDER "Engine")

target_include_directories(frame_graph_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(frame_graph_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(frame_graph_test PUBLIC EngineRuntime)
# target_compile_definitions(frame_graph_test PUBLIC UNIT_TEST)

set(POST_FG_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:frame_graph_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET frame_graph_test ${POST_FG_TEST_COMMANDS})
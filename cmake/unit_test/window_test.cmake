set(TARGET_NAME window_test)
add_executable(${TARGET_NAME} window_test.cpp)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "window_test")
# set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

target_include_directories(${TARGET_NAME} PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(${TARGET_NAME} PUBLIC EngineRuntime)

set(POST_WINDOW_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET_NAME}>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET ${TARGET_NAME} ${POST_WINDOW_TEST_COMMANDS})

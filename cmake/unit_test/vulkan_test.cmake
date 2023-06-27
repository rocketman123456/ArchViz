add_executable(vulkan_test vulkan_test.cpp)

set_target_properties(vulkan_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "window_test")
# set_target_properties(vulkan_test PROPERTIES FOLDER "Engine")

target_include_directories(vulkan_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(vulkan_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(vulkan_test PUBLIC EngineRuntime)

set(POST_VULKAN_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:vulkan_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET vulkan_test ${POST_VULKAN_TEST_COMMANDS})

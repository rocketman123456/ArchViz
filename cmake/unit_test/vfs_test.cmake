add_executable(vfs_test vfs_test.cpp)

set_target_properties(vfs_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "vfs_test")
# set_target_properties(vfs_test PROPERTIES FOLDER "Engine")

target_include_directories(vfs_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(vfs_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(vfs_test PUBLIC EngineRuntime)
# target_compile_definitions(vfs_test PUBLIC UNIT_TEST)

set(POST_VFS_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:vfs_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET vfs_test ${POST_VFS_TEST_COMMANDS})
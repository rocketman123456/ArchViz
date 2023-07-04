add_executable(gltf_test gltf_test.cpp)

set_target_properties(gltf_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "gltf_test")
# set_target_properties(gltf_test PROPERTIES FOLDER "Engine")

target_include_directories(gltf_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(gltf_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(gltf_test PUBLIC EngineRuntime tinygltf)
# target_compile_definitions(gltf_test PUBLIC UNIT_TEST)

set(POST_GLTF_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:gltf_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET gltf_test ${POST_GLTF_TEST_COMMANDS})

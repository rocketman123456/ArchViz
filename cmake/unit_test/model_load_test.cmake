add_executable(model_load_test model_load_test.cpp)

set_target_properties(model_load_test PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "model_load_test")
# set_target_properties(model_load_test PROPERTIES FOLDER "Engine")

target_include_directories(model_load_test PUBLIC ${ENGINE_ROOT_DIR}/source)
target_compile_options(model_load_test PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(model_load_test PUBLIC EngineRuntime)
# target_compile_definitions(model_load_test PUBLIC UNIT_TEST)

set(POST_MODEL_LOAD_TEST_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:model_load_test>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET model_load_test ${POST_MODEL_LOAD_TEST_COMMANDS})

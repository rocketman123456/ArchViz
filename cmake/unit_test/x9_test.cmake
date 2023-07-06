add_executable(x9_example_1 x9/x9_example_1.c)

target_include_directories(x9_example_1 PUBLIC ${THIRD_PARTY_DIR}/x9)
target_compile_options(x9_example_1 PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(x9_example_1 PUBLIC x9)

set(POST_X9_TEST_1_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:x9_example_1>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET x9_example_1 ${POST_X9_TEST_1_COMMANDS})

add_executable(x9_example_2 x9/x9_example_2.c)

target_include_directories(x9_example_2 PUBLIC ${THIRD_PARTY_DIR}/x9)
target_compile_options(x9_example_2 PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(x9_example_2 PUBLIC x9)

set(POST_X9_TEST_2_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:x9_example_2>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET x9_example_2 ${POST_X9_TEST_2_COMMANDS})

add_executable(x9_example_3 x9/x9_example_3.c)

target_include_directories(x9_example_3 PUBLIC ${THIRD_PARTY_DIR}/x9)
target_compile_options(x9_example_3 PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(x9_example_3 PUBLIC x9)

set(POST_X9_TEST_3_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:x9_example_3>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET x9_example_3 ${POST_X9_TEST_3_COMMANDS})

add_executable(x9_example_4 x9/x9_example_4.c)

target_include_directories(x9_example_4 PUBLIC ${THIRD_PARTY_DIR}/x9)
target_compile_options(x9_example_4 PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(x9_example_4 PUBLIC x9)

set(POST_X9_TEST_4_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:x9_example_4>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET x9_example_4 ${POST_X9_TEST_4_COMMANDS})

add_executable(x9_example_5 x9/x9_example_5.c)

target_include_directories(x9_example_5 PUBLIC ${THIRD_PARTY_DIR}/x9)
target_compile_options(x9_example_5 PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(x9_example_5 PUBLIC x9)

set(POST_X9_TEST_5_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:x9_example_5>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET x9_example_5 ${POST_X9_TEST_5_COMMANDS})

add_executable(x9_example_6 x9/x9_example_6.c)

target_include_directories(x9_example_6 PUBLIC ${THIRD_PARTY_DIR}/x9)
target_compile_options(x9_example_6 PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")
target_link_libraries(x9_example_6 PUBLIC x9)

set(POST_X9_TEST_6_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}/unit_test"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:x9_example_6>" "${BINARY_ROOT_DIR}/unit_test/"
)

add_custom_command(TARGET x9_example_6 ${POST_X9_TEST_6_COMMANDS})
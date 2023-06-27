option(TARGET_HELLO_WORLD "" OFF)
option(TARGET_PERFORMANCE_TEST "" OFF)
option(TARGET_SAMPLES "" OFF)
option(TARGET_UNIT_TESTS "" OFF)
option(TARGET_VIEWER "" OFF)
# option(CROSS_PLATFORM_DETERMINISTIC "" OFF)

if(ENABLE_PHYSICS_DEBUG_RENDERER)
    option(TARGET_TEST_FRAMEWORK "" ON)
else()
    option(TARGET_TEST_FRAMEWORK "" OFF)
endif()

add_subdirectory(JoltPhysics/Build)

target_compile_definitions(Jolt PUBLIC JPH_DISABLE_CUSTOM_ALLOCATOR)

if(ENABLE_PHYSICS_DEBUG_RENDERER)
    set_target_properties(Jolt TestFramework
        PROPERTIES 
        FOLDER ${third_party_folder}/JoltPhysics
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
else()
    set_target_properties(Jolt
        PROPERTIES 
        FOLDER ${third_party_folder}/JoltPhysics
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()
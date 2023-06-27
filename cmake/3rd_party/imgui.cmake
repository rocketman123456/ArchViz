set(imgui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR_}/*.cpp")
file(GLOB imgui_impl CONFIGURE_DEPENDS  
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.h"
)

add_library(imgui STATIC ${imgui_sources} ${imgui_impl})

target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR_}>)
# target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${vulkan_include}>)
# target_link_libraries(imgui PUBLIC glfw ${vulkan_lib})
target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${volk_include}>)

target_link_libraries(imgui PUBLIC volk::volk volk::volk_headers)
target_link_libraries(imgui PUBLIC glfw)

set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
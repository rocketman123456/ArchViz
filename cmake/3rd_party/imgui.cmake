set(imgui_SOURCE_DIR ${THIRD_PARTY_DIR}/imgui)

file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR}/*.cpp")
file(GLOB imgui_impl CONFIGURE_DEPENDS  
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp" 
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp" 
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h"
)

add_library(imgui STATIC ${imgui_sources} ${imgui_impl})

target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/imgui)
target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/volk)
target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/glfw/include)
target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/Vulkan-Headers/include)

set_target_properties(imgui PROPERTIES FOLDER ${THIRD_PARTY_DIR}/imgui)

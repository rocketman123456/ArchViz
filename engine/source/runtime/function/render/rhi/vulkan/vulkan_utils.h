#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace ArchViz
{
    struct QueueFamilyIndices;

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
}

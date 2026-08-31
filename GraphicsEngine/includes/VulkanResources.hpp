#pragma once

#include <algorithm>

#include "Window.hpp"
#include "VulkanData.hpp"
#include "ValidationLayers.hpp"
#include "ConsoleText.hpp"

const std::vector<const char*> deviceExtensions = {
    "VK_KHR_swapchain"
};

#ifdef __APPLE__
deviceExtensions.emplace_back("VK_KHR_portability_subset");
#endif

namespace Engine {

class VulkanResources {
public:
    VulkanResources() = default;
    
    void initVulkan(const char* appName, GLFWwindow* pGLFWwindow, bool enableValidationLayers);
    
    void createInstance(const char* appName);
    void createSurface(GLFWwindow* pGLFWwindow);
    void createLogicalDevice();
    void pickPhysicalDevice();
    
    const VkInstance& getInstance() { return instance; };
    const VkDevice& getLogicalDevice() { return logicalDevice; };
    const VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; };
    const VkSurfaceKHR& getSurface() { return surface; };
    const VkQueue& getGraphicsQueue() { return graphicsQueue; };
    const VkQueue& getPresentQueue() { return presentQueue; };
    const QueueFamilyIndices& getQueueFamilyIndices() { return indices; }
    
    ~VulkanResources();
    
private:
    void checkInstanceExtensionSupport();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getGLFWExtensions();
    
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    
    ValidationLayers validationLayers;
    QueueFamilyIndices indices;
    
    GLFWwindow* pGLFWwindow = nullptr;
    
    bool enableValidationLayers = false;
};

}

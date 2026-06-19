#pragma once

#include "VulkanData.hpp"
#include "ValidationLayers.hpp"
#include "ConsoleText.hpp"

namespace Engine {

const std::vector<const char*> deviceExtensions = {
    "VK_KHR_portability_subset",
    "VK_KHR_swapchain"
};

class VulkanResources {
public:
    VulkanResources() {};
    
    void initVulkan(const char* appName, GLFWwindow* pGLFWwindow, bool enableValidationLayers);
    
    void createInstance(const char* appName);
    void createSurface(GLFWwindow* pGLFWwindow);
    void createLogicalDevice();
    
    void pickPhysicalDevice();
    void checkInstanceExtensionSupport();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getGLFWExtensions();
    
    VkDevice& getLogicalDevice() { return logicalDevice; };
    VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; };
    VkSurfaceKHR& getSurface() { return surface; };
    
    void cleanUp();
    
    ~VulkanResources() {};
    
private:
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    
    ValidationLayers validationLayers;
    
    GLFWwindow* pGLFWwindow = nullptr;
    
    bool enableValidationLayers = false;
};

}

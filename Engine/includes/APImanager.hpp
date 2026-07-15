#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "VulkanResources.hpp"

namespace Engine {

class APImanager {
public:
    APImanager() = delete;
    APImanager(bool enableValidationLayers,
               GLFWwindow* pGLFWwindow,
               const int max_frames_in_flight);
    
    VulkanResources* getVulkanResources() { return pVulkanResources; };
    
    void initAPImanager(const char* appName);
    void drawFrame();
    
    ~APImanager();
    
private:
    VulkanResources* pVulkanResources;
    
    GLFWwindow* pGLFWwindow = nullptr;
    
    bool enableValidationLayers = false;
    int max_frames_in_flight = 0;
};

}

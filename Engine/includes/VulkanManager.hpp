#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "VulkanResources.hpp"
#include "SwapChain.hpp"

namespace Engine {

class Vulkan {
public:
    Vulkan(bool enableValidationLayers, GLFWwindow* pGLFWwindow);
    
    void createApplication(const char* appName);
    void cleanUp();
    
    ~Vulkan() { pGLFWwindow = nullptr; };
    
private:
    VulkanResources vulkanResources;
    SwapChain swapChain;
    
    GLFWwindow* pGLFWwindow = nullptr;
    
    bool enableValidationLayers = false;
};

}

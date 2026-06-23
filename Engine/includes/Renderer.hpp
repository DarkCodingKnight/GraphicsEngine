#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "ConsoleText.hpp"

namespace Engine {

class Renderer {
public:
    Renderer() {};
    
    void createRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat);
    const VkRenderPass& getRenderPass() { return renderPass; };
    void cleanUp(const VkDevice& device);
    
    ~Renderer() {};
    
private:
    VkRenderPass renderPass;
};

}

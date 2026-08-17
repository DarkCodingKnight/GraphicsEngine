#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "GraphicsObjectPool.hpp"

namespace Engine {

class Renderer : public WindowInterface
{
public:
    Renderer() = delete;
    Renderer(VulkanResources* vulkanResources,
             const int max_frames_in_flight,
             GLFWwindow* const pWindow);
    
    void initRenderer(const std::vector<char>& vertShaderCode,
                      const std::vector<char>& fragShaderCode);
    
    void recordCommandBuffer(std::vector<std::shared_ptr<GraphicsObjectPool>> pGraphicsObjectPools,
                             SwapChain* pSwapChain,
                             VkCommandBuffer& commandBuffer,
                             uint32_t imageIndex);
    
    void drawFrame(std::vector<std::shared_ptr<GraphicsObjectPool>> pGraphicsObjectPools,
                   SwapChain* pSwapChain);
    
    void updateWindow() override;
    VkCommandPool& getCommandPool() { return commandPool; };
    
    ~Renderer() override;
    
private:
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    
    VulkanResources* pVulkanResources = nullptr;
    
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    
    bool framebufferResized = false;
    int max_frames_in_flight = 0;
    uint32_t currentFrame = 0;
};

}

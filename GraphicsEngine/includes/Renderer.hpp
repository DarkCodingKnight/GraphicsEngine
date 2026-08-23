#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "GraphicsObjectPool.hpp"
#include "Camera.hpp"
#include "IMGUIwindow.hpp"

namespace Engine {

class Renderer : public WindowInterface
{
public:
    Renderer() = delete;
    Renderer(VulkanResources* _pVulkanResources_,
             Camera* _pCamera_,
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
    
    void createCommandPool(VkCommandPool* pCommandPool);
    void createCommandBuffers(VkCommandPool& commandPool,
                              VkCommandBuffer* pCommandBuffer,
                              uint32_t commandBuffersSize);
    
    void setIMGUI(IMGUIwindow* _pIMGUIwindow_) { pIMGUIwindow = _pIMGUIwindow_; };
    
    VkCommandPool& getCommandPool() { return commandPool; };
    
    void updateWindow() override;
    
    ~Renderer() override;
    
private:
    void createSyncObjects();
    
    VulkanResources* pVulkanResources = nullptr;
    Camera* pCamera = nullptr;
    IMGUIwindow* pIMGUIwindow = nullptr;
    
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

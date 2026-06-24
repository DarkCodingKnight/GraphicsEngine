#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "ConsoleText.hpp"
#include "VulkanData.hpp"

namespace Engine {

class Renderer {
public:
    Renderer() {};
    
    void createRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat);
    const VkRenderPass& getRenderPass() { return renderPass; };
    void cleanUp(const VkDevice& device);
    
    void createFramebuffers(const VkDevice& device,
                            const std::vector<VkImageView>& swapChainImageViews,
                            const VkExtent2D& swapChainImageExtent);
    void createCommandPool(const VkDevice& logicalDevice,
                           const VkPhysicalDevice& physicalDevice,
                           const VkSurfaceKHR& surface);
    void createCommandBuffer(const VkDevice& device);
    void recordCommandBuffer(const VkCommandBuffer& cBuffer,
                             const VkPipeline& pipeline,
                             uint32_t imageIndex,
                             const VkExtent2D& swapChainImageExtent);
    
    void drawFrame(const VkDevice& device,
                   const VkSwapchainKHR& swapChain,
                   const VkPipeline& pipeline,
                   const VkExtent2D& swapChainImageExtent,
                   const VkQueue& graphicsQueue,
                   const VkQueue& presentQueue);
    void createSyncObjects(const VkDevice& device);
    
    ~Renderer() {};
    
private:
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};

}

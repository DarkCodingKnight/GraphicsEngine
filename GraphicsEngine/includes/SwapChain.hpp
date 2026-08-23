#pragma once

#include "VulkanData.hpp"
#include "ConsoleText.hpp"
#include "VulkanResources.hpp"

namespace Engine {

class SwapChain {
public:
    SwapChain() = delete;
    SwapChain(VulkanResources* pResources,
              GLFWwindow* const pWindow);

    void createRenderPass();
    void createFramebuffers();
    void recreateSwapChain();
    void createDepthResources(VkSampleCountFlagBits sampleFlagBits);
    
    const VkSwapchainKHR& getSwapChain() { return swapChain; };
    const VkExtent2D& getSwapChainExtent() { return swapChainImageExtent; };
    const VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; };
    const std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; };
    const VkRenderPass& getSwapChainRenderPass() { return renderPass; };
    const std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return swapChainFramebuffers; };
    
    void cleanUp();
    
    ~SwapChain();

private:
    void createSwapChain();
    void createSwapChainImages(uint32_t* imageCount);
    void createImageViews();
    
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    VkExtent2D swapChainImageExtent;
    VkFormat swapChainImageFormat;
    
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    
    VkSwapchainKHR swapChain;
    
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    
    VulkanResources* pVulkanResources = nullptr;
    GLFWwindow* pGLFWwindow = nullptr;
};

}

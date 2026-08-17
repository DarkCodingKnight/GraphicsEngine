#pragma once

#include "VulkanData.hpp"
#include "ConsoleText.hpp"
#include "VulkanResources.hpp"

namespace Engine {

class SwapChain {
public:
    SwapChain() = delete;
    SwapChain(VulkanResources* pResources,
              GLFWwindow* const pWindow) :
    pVulkanResources(pResources), pGLFWwindow(pWindow) {};

    void createSwapChain();
    void createSwapChainImages(uint32_t* imageCount);
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void recreateSwapChain();
    void createDepthResources();
    
    const VkSwapchainKHR& getSwapChain() { return swapChain; };
    const VkExtent2D& getSwapChainExtent() { return swapChainImageExtent; };
    const VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; };
    const std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; };
    const VkRenderPass& getSwapChainRenderPass() { return renderPass; };
    const std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return swapChainFramebuffers; };
    
    ~SwapChain();

private:
    void cleanUp();
    
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

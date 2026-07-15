#pragma once

#include "VulkanData.hpp"
#include "ConsoleText.hpp"
#include "APImanager.hpp"

namespace Engine {

class SwapChain {
public:
    SwapChain() = delete;
    SwapChain(APImanager* pManager, GLFWwindow* const pWindow) : pAPImanager(pManager), pGLFWwindow(pWindow) {};

    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void recreateSwapChain();
    
    const VkSwapchainKHR& getSwapChain() { return swapChain; };
    const VkExtent2D& getSwapChainExtent() { return swapChainImageExtent; };
    const VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; };
    const std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; };
    const VkRenderPass& getSwapChainRenderPass() { return renderPass; };
    const std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return swapChainFramebuffers; };
    void getSwapChainImages(uint32_t* imageCount);

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
    
    APImanager* pAPImanager = nullptr;
    GLFWwindow* pGLFWwindow = nullptr;
};

}

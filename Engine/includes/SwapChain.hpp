#pragma once

#include "VulkanData.hpp"
#include "ConsoleText.hpp"

namespace Engine {

class SwapChain {
public:
    SwapChain(GLFWwindow* pWindow) : pGLFWindow(pWindow) {};

    void createSwapChain(const VkDevice& logicalDevice,
                         const VkPhysicalDevice& physicalDevice,
                         const VkSurfaceKHR& surface);
    const VkExtent2D& getSwapChainExtent() { return swapChainImageExtent; };
    const VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; };
    const std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; };
    const VkSwapchainKHR& getSwapChain() { return swapChain; };
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void getSwapChainImages(const VkDevice& device, uint32_t* imageCount);
    void createImageViews(const VkDevice& device);
    void cleanUp(const VkDevice& device);

    ~SwapChain() { pGLFWindow = nullptr; };

private:
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    VkExtent2D swapChainImageExtent;
    VkFormat swapChainImageFormat;
    
    VkSwapchainKHR swapChain;
    GLFWwindow* pGLFWindow = nullptr;
};

}

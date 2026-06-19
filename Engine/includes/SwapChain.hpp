#pragma once

#include "VulkanData.hpp"
#include "ConsoleText.hpp"

namespace Engine {

class SwapChain {
public:
    SwapChain(GLFWwindow* pWindow) : pGLFWindow(pWindow) {};

    void createSwapChain(VkDevice& logicalDevice,
                         VkPhysicalDevice& physicalDevice,
                         VkSurfaceKHR& surface);
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void getSwapChainImages(VkDevice& device, uint32_t* imageCount);
    void createImageViews(VkDevice& device);
    void cleanUp(VkDevice& device);

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

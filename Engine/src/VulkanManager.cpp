#include "VulkanManager.hpp"

namespace Engine {

Vulkan::Vulkan(bool enableValidationLayers, GLFWwindow* pGLFWwindow) : vulkanResources(VulkanResources()), swapChain(pGLFWwindow) {
    this->enableValidationLayers = enableValidationLayers;
    this->pGLFWwindow = pGLFWwindow;
}

void Vulkan::createApplication(const char* appName) {
    vulkanResources.initVulkan(appName, pGLFWwindow, enableValidationLayers);
    vulkanResources.createSurface(pGLFWwindow);
    vulkanResources.pickPhysicalDevice();
    vulkanResources.createLogicalDevice();
    
    swapChain.createSwapChain(vulkanResources.getLogicalDevice(),
                              vulkanResources.getPhysicalDevice(),
                              vulkanResources.getSurface());
    swapChain.createImageViews(vulkanResources.getLogicalDevice());
}

void Vulkan::cleanUp() {
    swapChain.cleanUp(vulkanResources.getLogicalDevice());
    vulkanResources.cleanUp();
}

}

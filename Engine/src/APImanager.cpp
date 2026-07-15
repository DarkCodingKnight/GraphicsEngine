#include "APImanager.hpp"

namespace Engine {

APImanager::APImanager(bool enableValidationLayers,
                       GLFWwindow* pGLFWwindow,
                       const int max_frames_in_flight)
{
    pVulkanResources = new VulkanResources();
    
    this->enableValidationLayers = enableValidationLayers;
    this->pGLFWwindow = pGLFWwindow;
}

void APImanager::initAPImanager(const char* appName) {
    pVulkanResources->initVulkan(appName, pGLFWwindow, enableValidationLayers);
    pVulkanResources->createSurface(pGLFWwindow);
    pVulkanResources->pickPhysicalDevice();
    pVulkanResources->createLogicalDevice();
}

APImanager::~APImanager() {
    delete pVulkanResources;
    
    pGLFWwindow = nullptr;
}

}

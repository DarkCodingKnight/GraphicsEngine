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
    
    
    resourceManager = ResourceManager();
    
    const std::string vertFilePath = "/Users/user/Projects/GraphicsEngine/Engine/shaders/vert.spv";
    const std::string fragFilePath = "/Users/user/Projects/GraphicsEngine/Engine/shaders/frag.spv";
    
    const std::string vertFilePath1 = "/../../../GraphicsEngine/Engine/shaders/vert.spv";
    const std::string fragFilePath1 = "/../../../GraphicsEngine/Engine/shaders/frag.spv";
    
    std::vector<char> vertCode;
    std::optional<std::vector<char>> vertEnableCode = resourceManager.readFile(vertFilePath);
    if (vertEnableCode.has_value()) vertCode = vertEnableCode.value();
    
    std::vector<char> fragCode;
    std::optional<std::vector<char>> fragEnableCode = resourceManager.readFile(fragFilePath);
    if (fragEnableCode.has_value()) fragCode = fragEnableCode.value();
    
    renderer = Renderer();
    renderer.createRenderPass(vulkanResources.getLogicalDevice(), swapChain.getSwapChainImageFormat());
    
    pipeline = Pipeline();
    pipeline.createGraphicsPipeline(vulkanResources.getLogicalDevice(), renderer.getRenderPass(), vertCode, fragCode, swapChain.getSwapChainExtent());
}

void Vulkan::cleanUp() {
    pipeline.cleanUp(vulkanResources.getLogicalDevice());
    renderer.cleanUp(vulkanResources.getLogicalDevice());
    swapChain.cleanUp(vulkanResources.getLogicalDevice());
    vulkanResources.cleanUp();
}

}

#include "GraphicsApplication.hpp"

namespace Engine {

GraphicsApplication::GraphicsApplication(GraphicsApplicationInfo* pAppInfo)
{
    ConsoleText::printGreen("Graphics application initialization started!", "Graphics application");
    
    pWindow = new Window(pAppInfo->window_width,
                         pAppInfo->window_height,
                         pAppInfo->application_name);
    pResourceManager = new ResourceManager();
    
    // -APImanager
    
    pVulkanResources = new VulkanResources();
    pVulkanResources->initVulkan(pAppInfo->application_name,
                                 pWindow->getGLFWwindowPtr(),
                                 pAppInfo->enable_validation_Layers);
    pVulkanResources->createSurface(pWindow->getGLFWwindowPtr());
    pVulkanResources->pickPhysicalDevice();
    pVulkanResources->createLogicalDevice();
    
    pSwapChain = new SwapChain(pVulkanResources, pWindow->getGLFWwindowPtr());
    pSwapChain->createSwapChain();
    pSwapChain->createImageViews();
    pSwapChain->createDepthResources();
    pSwapChain->createRenderPass();
    pSwapChain->createFramebuffers();
    
    pRenderer = new Renderer(pVulkanResources,
                             pAppInfo->max_frames_in_flight,
                             pWindow->getGLFWwindowPtr());
    
    pWindow->setFramebufferUpdateSubscribers(pRenderer);
    pWindow->setKeyCallbackSubscribers(pResourceManager);
}

bool GraphicsApplication::updateGraphicsApplication() {
    if(!pGraphicsObjectPools.empty() && !pWindow->isWindowClose()) {
        pRenderer->drawFrame(pGraphicsObjectPools, pSwapChain);
        return true;
    }
    else {
        cleanUpApplication();
        return false;
    }
}

void GraphicsApplication::cleanUpApplication() {
    for(std::shared_ptr<GraphicsObjectPool> pGraphicsObjectPool : pGraphicsObjectPools)
    {
        for(std::shared_ptr<GraphicsObject> pGraphicsObject : pGraphicsObjectPool->getGraphicsObjects())
        {
            pGraphicsObject.reset();
        }
        pGraphicsObjectPool.reset();
    }
}

std::weak_ptr<GraphicsObjectPool> GraphicsApplication::createGraphicsObjectPool(ObjectPoolInfo* pPoolInfo)
{
    std::shared_ptr<GraphicsObjectPool> pObjectPool(new GraphicsObjectPool(pVulkanResources,
                                                                           pSwapChain,
                                                                           pRenderer->getCommandPool(),
                                                                           pPoolInfo->vertexPath,
                                                                           pPoolInfo->fragmentPath,
                                                                           pWindow->getGLFWwindowPtr(),
                                                                           pPoolInfo->max_frames_in_flight));
    
    pGraphicsObjectPools.push_back(pObjectPool);
    return pObjectPool;
}

GraphicsApplication::~GraphicsApplication() {
    delete pRenderer;
    delete pSwapChain;
    delete pVulkanResources;
    
    pWindow->destroyWindow();
    
    delete pWindow;
    delete pResourceManager;
    
    pRenderer = nullptr;
    pSwapChain = nullptr;
    pVulkanResources = nullptr;
    pWindow = nullptr;
    pResourceManager = nullptr;
}

}

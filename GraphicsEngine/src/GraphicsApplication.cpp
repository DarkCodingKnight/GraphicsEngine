#include "GraphicsApplication.hpp"

namespace Engine {

GraphicsApplication::GraphicsApplication(GraphicsApplicationInfo* pAppInfo)
{
    ConsoleText::printGreen("Graphics application initialization started!", "Graphics application");
    
    pWindow = new Window(pAppInfo->window_width,
                         pAppInfo->window_height,
                         pAppInfo->application_name);
    pResourceManager = new ResourceManager();
    
    pVulkanResources = new VulkanResources();
    pVulkanResources->initVulkan(pAppInfo->application_name,
                                 pWindow->getGLFWwindowPtr(),
                                 pAppInfo->enable_validation_Layers);
    pVulkanResources->createSurface(pWindow->getGLFWwindowPtr());
    pVulkanResources->pickPhysicalDevice();
    pVulkanResources->createLogicalDevice();
    
    pSwapChain = new SwapChain(pVulkanResources, pWindow->getGLFWwindowPtr());
    
    pSampler = new Sampler(pVulkanResources, pSwapChain);
    
    pSwapChain->createDepthResources(pSampler->getSampleFlagBits());
    pSwapChain->createRenderPass();
    pSwapChain->createFramebuffers();
    
    pCamera = new Camera(pResourceManager,
                         pSwapChain->getSwapChainExtent().width,
                         pSwapChain->getSwapChainExtent().height,
                         0.5f, 1000.0f, 45.0f);
    pCamera->setEnableRenderTime(true);
    
    pRenderer = new Renderer(pVulkanResources,
                             pCamera,
                             pAppInfo->max_frames_in_flight,
                             pWindow->getGLFWwindowPtr());
    
    pWindow->setFramebufferUpdateSubscribers(pRenderer);
    pWindow->setKeyCallbackSubscribers(pResourceManager);
    
    pDescriptorResources = new DescriptorResources(pVulkanResources, pAppInfo->max_frames_in_flight);
    
    pIMGUIwindow = new IMGUIwindow(pVulkanResources,
                                   pSwapChain,
                                   pDescriptorResources,
                                   pWindow->getGLFWwindowPtr());
    
    pRenderer->setIMGUI(pIMGUIwindow);
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

std::weak_ptr<GraphicsObjectPool> GraphicsApplication::createGraphicsObjectPool(ObjectPoolInfo* pPoolInfo)
{
    std::shared_ptr<GraphicsObjectPool> pObjectPool(new GraphicsObjectPool(pVulkanResources,
                                                                           pDescriptorResources,
                                                                           pSampler,
                                                                           pSwapChain,
                                                                           pRenderer->getCommandPool(),
                                                                           pPoolInfo->vertexPath,
                                                                           pPoolInfo->fragmentPath,
                                                                           pWindow->getGLFWwindowPtr(),
                                                                           pPoolInfo->max_frames_in_flight));
    
    pGraphicsObjectPools.push_back(pObjectPool);
    return pObjectPool;
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

GraphicsApplication::~GraphicsApplication() {
    delete pSampler;
    delete pDescriptorResources;
    delete pRenderer;
    delete pSwapChain;
    delete pVulkanResources;
    
    pWindow->destroyWindow();
    
    delete pWindow;
    delete pResourceManager;
    
    pDescriptorResources = nullptr;
    pRenderer = nullptr;
    pSwapChain = nullptr;
    pVulkanResources = nullptr;
    pWindow = nullptr;
    pResourceManager = nullptr;
}

}

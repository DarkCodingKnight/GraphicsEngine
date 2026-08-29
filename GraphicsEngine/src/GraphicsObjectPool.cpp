#include "GraphicsObjectPool.hpp"

namespace Engine {

GraphicsObjectPool::GraphicsObjectPool(VulkanResources* _pVulkanResources_,
                                       DescriptorResources* _pDescriptorResources_,
                                       Sampler* _pSampler_,
                                       SwapChain* _pSwapChain_,
                                       IMGUIwindow* _pIMGUIwindow_,
                                       VkCommandPool& _commandPool_,
                                       std::string vertexPath,
                                       std::string fragmentPath,
                                       const GLFWwindow* pGLFWwindow,
                                       uint16_t max_frames) :
pVulkanResources(_pVulkanResources_), pDescriptorResources(_pDescriptorResources_), pSampler(_pSampler_),
max_frames_in_flight(max_frames), commandPool(_commandPool_), pIMGUIwindow(_pIMGUIwindow_)
{
    std::vector<char> vertCode{};
    std::vector<char> fragCode{};
    
    std::optional<std::vector<char>> vertEnableCode = ResourceManager::readFile(vertexPath);
    if (vertEnableCode.has_value()) vertCode = vertEnableCode.value();
    
    std::optional<std::vector<char>> fragEnableCode = ResourceManager::readFile(fragmentPath);
    if (fragEnableCode.has_value()) fragCode = fragEnableCode.value();
    
    pPipeline = new Pipeline(pVulkanResources,
                             _pSwapChain_,
                             pDescriptorResources->getDescriptorSetLayout(),
                             pGLFWwindow,
                             vertCode,
                             fragCode,
                             max_frames_in_flight);
    
    pDescriptorPool = pDescriptorResources->getEngineDescriptorPool();
}

std::weak_ptr<GraphicsObject> GraphicsObjectPool::createGraphicsObject(const char* modelPath,
                                                                       const char* texturePath,
                                                                       std::string object_name)
{
    std::shared_ptr<GraphicsObject> pGraphicsObject =
    std::make_shared<GraphicsObject>(pVulkanResources,
                                     commandPool,
                                     *(pDescriptorResources->getDescriptorSetLayout()),
                                     pDescriptorPool->getdescriptorPool(),
                                     pSampler->getSampler(),
                                     texturePath,
                                     modelPath,
                                     max_frames_in_flight,
                                     object_name);
    
    pGraphicsObjects.push_back(pGraphicsObject);
    pIMGUIwindow->setGraphicsObject(pGraphicsObject);
    
    return pGraphicsObject;
}

GraphicsObjectPool::~GraphicsObjectPool() {
    delete pPipeline;
    
    pPipeline = nullptr;
    pSampler = nullptr;
    pDescriptorResources = nullptr;
    pVulkanResources = nullptr;
}

}

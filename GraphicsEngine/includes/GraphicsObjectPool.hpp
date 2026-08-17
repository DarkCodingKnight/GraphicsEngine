#pragma once

#include <glm/glm.hpp>

#include "Pipeline.hpp"
#include "GraphicsObject.hpp"

namespace Engine {

class GraphicsObjectPool
{
public:
    GraphicsObjectPool() = delete;
    GraphicsObjectPool(const GraphicsObjectPool& other) = delete;
    GraphicsObjectPool(GraphicsObjectPool&& other) = delete;
    GraphicsObjectPool& operator=(const GraphicsObjectPool& other) = delete;
    GraphicsObjectPool& operator=(GraphicsObjectPool&& other) = delete;
    
    GraphicsObjectPool(VulkanResources* vulkanResources,
                       SwapChain* swapChain,
                       VkCommandPool& comPool,
                       std::string vertexPath,
                       std::string fragmentPath,
                       const GLFWwindow* pGLFWwindow,
                       uint16_t max_frames);
    
    std::weak_ptr<GraphicsObject> createGraphicsObject(const char* modelPath,
                                                       const char* texturePath);
    
    Pipeline* getPipeline() { return pPipeline; };
    
    std::vector<std::shared_ptr<GraphicsObject>>& getGraphicsObjects() {
        if(pGraphicsObjects.empty()) {
            ConsoleText::printError("You must create object!", "ObjectPool");
            std::exit(0);
        }
        else return pGraphicsObjects;
    };
    
    ~GraphicsObjectPool();
    
private:
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createTextureSampler();
    
    uint16_t max_frames_in_flight = 0;
    
    VulkanResources* pVulkanResources = nullptr;
    Pipeline* pPipeline = nullptr;
    
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkSampler textureSampler;
    VkCommandPool commandPool;
    
    std::vector<std::shared_ptr<GraphicsObject>> pGraphicsObjects;
};

}

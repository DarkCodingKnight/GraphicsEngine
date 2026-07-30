#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "APImanager.hpp"
#include "ConsoleText.hpp"
#include "Window.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"

namespace Engine {

class Renderer : public WindowInterface
{
public:
    Renderer() = delete;
    Renderer(APImanager* pManager,
             const int max_frames_in_flight,
             GLFWwindow* const pWindow) :
    pAPImanager(pManager)
    {
        if (max_frames_in_flight == 0) std::exit(0);
        else this->max_frames_in_flight = max_frames_in_flight;
        
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();
    };
    
    void initRenderer(const std::vector<char>& vertShaderCode,
                      const std::vector<char>& fragShaderCode);
    
    void recordCommandBuffer(Pipeline* pPipeline,
                             Mesh* pMesh,
                             const VkCommandBuffer& cBuffer,
                             const VkPipeline& pipeline,
                             uint32_t imageIndex);
    
    const VkCommandPool& getCommandPool() { return commandPool; };
    
    void drawFrame(Pipeline* pPipeline, Mesh* pMesh);
    void updateWindow() override;
    
    ~Renderer() override;
    
private:
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    
    APImanager* pAPImanager = nullptr;
    
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    
    bool framebufferResized = false;
    int max_frames_in_flight = 0;
    uint32_t currentFrame = 0;
};

}

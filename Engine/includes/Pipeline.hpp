#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "ConsoleText.hpp"
#include "SwapChain.hpp"
#include "Mesh.hpp"

namespace Engine {

class Pipeline {
public:
    Pipeline() = delete;
    Pipeline(APImanager* pManager, GLFWwindow* pGLFWwndow, const std::vector<char> vertCode, const std::vector<char> fragCode);
    
    VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& shaderCode);
    const VkPipeline& getPipeline() { return graphicsPipeline; };
    SwapChain* getSwapChain() { return pSwapChain; };
    
    ~Pipeline();
    
private:
    VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
    
    VkPipelineDynamicStateCreateInfo createDynamicState(const std::vector<VkDynamicState>& dynamicStates);
    VkPipelineInputAssemblyStateCreateInfo createPipelineInputAssemble();
    VkPipelineViewportStateCreateInfo createViewportState(const VkViewport* viewport, const VkRect2D* scissor);
    VkViewport initViewport(const VkExtent2D& swapChainExtent);
    VkRect2D initScissor(const VkExtent2D& swapChainExtent);
    VkPipelineRasterizationStateCreateInfo createRasterizationStage();
    VkPipelineMultisampleStateCreateInfo createMultisamplingStage();
    VkPipelineColorBlendAttachmentState initColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo createColorBlendState(VkPipelineColorBlendAttachmentState* colorBlendAttachmentState);
    void createPipelineLayout(const VkDevice& device);
    
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    APImanager* pAPImanager = nullptr;
    SwapChain* pSwapChain = nullptr;
};

}

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "ConsoleText.hpp"

namespace Engine {

class Pipeline {
public:
    Pipeline() {};
    
    void createGraphicsPipeline(const VkDevice& device,
                                const VkRenderPass& renderPass,
                                const std::vector<char>& vertShaderCode,
                                const std::vector<char>& fragShaderCode,
                                const VkExtent2D& swapChainExtent);
    VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& shaderCode);
    void cleanUp(const VkDevice& device);
    
    ~Pipeline() {};
    
private:
    VkPipelineDynamicStateCreateInfo createDynamicState(const std::vector<VkDynamicState>& dynamicStates);
    VkPipelineVertexInputStateCreateInfo createVertexInput();
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
};

}

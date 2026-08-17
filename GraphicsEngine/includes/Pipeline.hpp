#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "SwapChain.hpp"
#include "Mesh.hpp"

namespace Engine {

class Pipeline {
public:
    Pipeline() = delete;
    Pipeline(VulkanResources* vulkanResources,
             SwapChain* swapChain,
             VkDescriptorSetLayout* descriptorSetLayout,
             const GLFWwindow* pGLFWwndow,
             const std::vector<char> vertCode,
             const std::vector<char> fragCode,
             const int _frames_);
    
    const VkPipeline& getPipeline() { return graphicsPipeline; };
    SwapChain* getSwapChain() { return pSwapChain; };
    const VkPipelineLayout& getPipelineLayout() { return pipelineLayout; };
    
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
    void createPipelineLayout(VkDescriptorSetLayout* descriptorSetLayout);
    
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
};

}

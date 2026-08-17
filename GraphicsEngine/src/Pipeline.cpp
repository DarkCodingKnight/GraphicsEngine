#include "Pipeline.hpp"

namespace Engine {

Pipeline::Pipeline(VulkanResources* vulkanResources,
                   SwapChain* swapChain,
                   VkDescriptorSetLayout* descriptorSetLayout,
                   const GLFWwindow* pGLFWwndow,
                   const std::vector<char> vertCode,
                   const std::vector<char> fragCode,
                   const int _frames_) :
pVulkanResources(vulkanResources), pSwapChain(swapChain)
{
    VkShaderModule vertShaderModule = createShaderModule(vertCode);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    if (vertShaderModule != NULL) {
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
    }
    
    VkShaderModule fragShaderModule = createShaderModule(fragCode);
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    if (fragShaderModule != NULL) {
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
    }
    
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    
    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = createDynamicState(dynamicStates);
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssembleCreateInfo = createPipelineInputAssemble();
    VkViewport viewport = initViewport(pSwapChain->getSwapChainExtent());
    VkRect2D scissor = initScissor(pSwapChain->getSwapChainExtent());
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = createViewportState(&viewport, &scissor);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = createRasterizationStage();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = createMultisamplingStage();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = initColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = createColorBlendState(&colorBlendAttachmentState);
    
    createPipelineLayout(descriptorSetLayout);
    
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    
    // binding buffers
    auto bindingDescription = getBindingDescription();
    auto attributeDescriptions = getAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssembleCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;
    depthStencilInfo.depthWriteEnable = VK_TRUE;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.minDepthBounds = 0.0f;
    depthStencilInfo.maxDepthBounds = 1.0f;
    depthStencilInfo.stencilTestEnable = VK_FALSE;
    depthStencilInfo.front = {};
    depthStencilInfo.back = {};
    
    pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = pSwapChain->getSwapChainRenderPass();
    pipelineCreateInfo.subpass = 0;
    //pipelineCreateInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT; // if you want to inherit pipeline
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;
    
    if (vkCreateGraphicsPipelines(pVulkanResources->getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create graphics pipeline!", "Pipeline");
        
        vkDestroyShaderModule(pVulkanResources->getLogicalDevice(), vertShaderModule, nullptr);
        ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
        vkDestroyShaderModule(pVulkanResources->getLogicalDevice(), fragShaderModule, nullptr);
        ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
        
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Graphics pipeline was created!", "Pipeline");
    }
    
    vkDestroyShaderModule(pVulkanResources->getLogicalDevice(), vertShaderModule, nullptr);
    ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
    vkDestroyShaderModule(pVulkanResources->getLogicalDevice(), fragShaderModule, nullptr);
    ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& shaderCode) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    VkShaderModule shaderModule;
    
    if (vkCreateShaderModule(pVulkanResources->getLogicalDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create shader program!", "Pipeline");
    }
    else {
        ConsoleText::printGreen("Shader programm was created!", "Pipeline");
    }
    return shaderModule;
}

VkPipelineDynamicStateCreateInfo Pipeline::createDynamicState(const std::vector<VkDynamicState>& dynamicStates) {
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
    
    return dynamicStateCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo Pipeline::createPipelineInputAssemble() {
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssembleCreateInfo{};
    pipelineInputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
    
    return pipelineInputAssembleCreateInfo;
}

VkPipelineColorBlendStateCreateInfo Pipeline::createColorBlendState(VkPipelineColorBlendAttachmentState* colorBlendAttachmentState) {
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = colorBlendAttachmentState;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;
    colorBlendCreateInfo.blendConstants[3] = 0.0f;
    
    return colorBlendCreateInfo;
}

VkPipelineColorBlendAttachmentState Pipeline::initColorBlendAttachmentState() {
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                               VK_COLOR_COMPONENT_G_BIT |
                                               VK_COLOR_COMPONENT_B_BIT |
                                               VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    
    // Color blending example.
    //colorBlendAttachment.blendEnable = VK_TRUE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    return colorBlendAttachmentState;
}

VkPipelineMultisampleStateCreateInfo Pipeline::createMultisamplingStage() {
    VkPipelineMultisampleStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.sampleShadingEnable = VK_FALSE;
    createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.minSampleShading = 1.0f;
    createInfo.pSampleMask = nullptr;
    createInfo.alphaToCoverageEnable = VK_FALSE;
    createInfo.alphaToOneEnable = VK_FALSE;
    
    return createInfo;
}

VkPipelineRasterizationStateCreateInfo Pipeline::createRasterizationStage() {
    VkPipelineRasterizationStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = VK_POLYGON_MODE_FILL;
    createInfo.lineWidth = 1.0f;
    createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    createInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    createInfo.depthBiasEnable = VK_FALSE;
    createInfo.depthBiasConstantFactor = 0.0f;
    createInfo.depthBiasClamp = 0.0f;
    createInfo.depthBiasSlopeFactor = 0.0f;
    
    return createInfo;
}

VkViewport Pipeline::initViewport(const VkExtent2D& swapChainExtent) {
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    return viewport;
}

VkRect2D Pipeline::initScissor(const VkExtent2D& swapChainExtent) {
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    
    return scissor;
}

VkPipelineViewportStateCreateInfo Pipeline::createViewportState(const VkViewport* viewport, const VkRect2D* scissor) {
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = scissor;
    
    return viewportStateCreateInfo;
}

void Pipeline::createPipelineLayout(VkDescriptorSetLayout* descriptorSetLayout) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(pVulkanResources->getLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create pipeline layout!", "Pipeline");
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Pipeline layout was created!", "Pipeline");
    }
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(pVulkanResources->getLogicalDevice(), graphicsPipeline, nullptr);
    ConsoleText::printGreen("Pipeline was destroyed!", "Pipeline");
    
    vkDestroyPipelineLayout(pVulkanResources->getLogicalDevice(), pipelineLayout, nullptr);
    ConsoleText::printGreen("Pipeline layout was destroyed!", "Pipeline");
    
    pSwapChain = nullptr;
    pVulkanResources = nullptr;
}

}

#include "Pipeline.hpp"

namespace Engine {

void Pipeline::createGraphicsPipeline(const VkDevice& device,
                                      const VkRenderPass& renderPass,
                                      const std::vector<char>& vertShaderCode,
                                      const std::vector<char>& fragShaderCode,
                                      const VkExtent2D& swapChainExtent)
{
    VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    if (vertShaderModule != NULL) {
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
    }
    
    VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);
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
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = createVertexInput();
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssembleCreateInfo = createPipelineInputAssemble();
    VkViewport viewport = initViewport(swapChainExtent);
    VkRect2D scissor = initScissor(swapChainExtent);
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = createViewportState(&viewport, &scissor);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = createRasterizationStage();
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = createMultisamplingStage();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = initColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = createColorBlendState(&colorBlendAttachmentState);
    
    createPipelineLayout(device);
    
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssembleCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    //pipelineCreateInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT; // if you want to inherit pipeline
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;
    
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create graphics pipeline!", "Pipeline");
        
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
        ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
        
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Graphics pipeline was created!", "Pipeline");
    }
    
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    ConsoleText::printGreen("Shader module was destroyed!", "Pipeline");
}

VkPipelineDynamicStateCreateInfo Pipeline::createDynamicState(const std::vector<VkDynamicState>& dynamicStates) {
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
    
    return dynamicStateCreateInfo;
}

VkPipelineVertexInputStateCreateInfo Pipeline::createVertexInput() {
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    
    return vertexInputInfo;
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
    createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

VkShaderModule Pipeline::createShaderModule(const VkDevice& device, const std::vector<char>& shaderCode) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderCode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    VkShaderModule shaderModule;
    
    if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create shader program!", "Pipeline");
    }
    else {
        ConsoleText::printGreen("Shader programm was created!", "Pipeline");
    }
    return shaderModule;
}

void Pipeline::createPipelineLayout(const VkDevice& device) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create pipeline layout!", "Pipeline");
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Pipeline layout was created!", "Pipeline");
    }
}

void Pipeline::cleanUp(const VkDevice& device) {
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    ConsoleText::printGreen("Pipeline was destroyed!", "Pipeline");
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    ConsoleText::printGreen("Pipeline layout was destroyed!", "Pipeline");
}

}

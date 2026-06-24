#include "Renderer.hpp"

namespace Engine {

void Renderer::createRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    
    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;
    
    if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create render pass", "Renderer");
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Render pass was created!", "Renderer");
    }
}

void Renderer::createFramebuffers(const VkDevice& device,
                                  const std::vector<VkImageView>& swapChainImageViews,
                                  const VkExtent2D& swapChainImageExtent) {
    swapChainFramebuffers.resize(swapChainImageViews.size());
    
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = { swapChainImageViews[i] };
        
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = swapChainImageExtent.width;
        framebufferCreateInfo.height = swapChainImageExtent.height;
        framebufferCreateInfo.layers = 1;
        
        if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(swapChainFramebuffers.at(i))) != VK_SUCCESS) {
            std::string message = "Failed to create framebuffer " + std::to_string(i + 1) + ".";
            ConsoleText::printError(message, "Renderer");
        }
        else {
            std::string message = "Framebuffer " + std::to_string(i + 1) + " was created.";
            ConsoleText::printGreen(message, "Renderer");
        }
    }
}

void Renderer::createCommandPool(const VkDevice& logicalDevice,
                                 const VkPhysicalDevice& physicalDevice,
                                 const VkSurfaceKHR& surface) {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);
    
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    if (vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create command pool!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was created!", "Renderer");
}

void Renderer::createCommandBuffer(const VkDevice& device) {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    
    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was allocated!", "Renderer");
}

void Renderer::recordCommandBuffer(const VkCommandBuffer& cBuffer,
                                   const VkPipeline& pipeline,
                                   uint32_t imageIndex,
                                   const VkExtent2D& swapChainImageExtent) {
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    
    if (vkBeginCommandBuffer(cBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
        ConsoleText::printError("Failed to begin command buffer!", "Renderer");
        return;
    }
    else ConsoleText::printGreen("Command buffer was begined!", "Renderer");
    
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = swapChainFramebuffers.at(imageIndex);
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = swapChainImageExtent;
    
    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(cBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainImageExtent.width);
    viewport.height = static_cast<float>(swapChainImageExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cBuffer, 0, 1, &viewport);
    
    VkRect2D scissor{};
    scissor.offset =  { 0, 0 };
    scissor.extent = swapChainImageExtent;
    vkCmdSetScissor(cBuffer, 0, 1, &scissor);
    
    vkCmdDraw(cBuffer, 3, 1, 0, 0); // cBuffer, vertexCount, instanceCount, firstVertex, firstInstance
    vkCmdEndRenderPass(cBuffer);
    
    if (vkEndCommandBuffer(cBuffer) != VK_SUCCESS) {
        ConsoleText::printError("Failed to record command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was recorder!", "Renderer");
}

void Renderer::drawFrame(const VkDevice& device,
                         const VkSwapchainKHR& swapChain,
                         const VkPipeline& pipeline,
                         const VkExtent2D& swapChainImageExtent,
                         const VkQueue& graphicsQueue,
                         const VkQueue& presentQueue) {
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, pipeline, imageIndex, swapChainImageExtent);
    
    // Command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        ConsoleText::printError("Failed to submit draw command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Draw command buffer was submited!", "Renderer");
    
    // Presentation
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;
    
    vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Renderer::createSyncObjects(const VkDevice& device) {
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create sync objects!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Sync objects were created!", "Renderer");
}

void Renderer::cleanUp(const VkDevice& device) {
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
    ConsoleText::printGreen("Sync objects were destroyed!", "Renderer");
    
    vkDestroyCommandPool(device, commandPool, nullptr);
    ConsoleText::printGreen("Command pool was destroyed!", "Renderer");
    
    for (int i = 0; i < swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(device, swapChainFramebuffers.at(i), nullptr);
        std::string message = "Framebuffer " + std::to_string(i + 1) + " was destroyed.";
        ConsoleText::printGreen(message, "Renderer");
    }
    
    vkDestroyRenderPass(device, renderPass, nullptr);
    ConsoleText::printGreen("Render pass was destroyed!", "Renderer");
}

}

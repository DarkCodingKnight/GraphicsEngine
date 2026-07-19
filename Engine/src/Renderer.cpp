#include "Renderer.hpp"

namespace Engine {

void Renderer::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(pAPImanager->getVulkanResources()->getPhysicalDevice(),
                          pAPImanager->getVulkanResources()->getSurface());
    
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    if (vkCreateCommandPool(pAPImanager->getVulkanResources()->getLogicalDevice(), &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create command pool!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was created!", "Renderer");
}

void Renderer::createCommandBuffers() {
    commandBuffers.resize(max_frames_in_flight);
    
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();
    
    if (vkAllocateCommandBuffers(pAPImanager->getVulkanResources()->getLogicalDevice(), &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was allocated!", "Renderer");
}

void Renderer::recordCommandBuffer(Pipeline* pPipeline,
                                   Mesh* pMesh,
                                   const VkCommandBuffer& cBuffer,
                                   const VkPipeline& pipeline,
                                   uint32_t imageIndex) {
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(cBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
        ConsoleText::printError("Failed to begin command buffer!", "Renderer");
        return;
    }
    else ConsoleText::printGreen("Command buffer was begined!", "Renderer");
    
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = pPipeline->getSwapChain()->getSwapChainRenderPass();
    renderPassBeginInfo.framebuffer = (pPipeline->getSwapChain()->getSwapChainFramebuffers()).at(imageIndex);
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = pPipeline->getSwapChain()->getSwapChainExtent();
    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(cBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    vkCmdBindPipeline(cBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(pPipeline->getSwapChain()->getSwapChainExtent().width);
    viewport.height = static_cast<float>(pPipeline->getSwapChain()->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    vkCmdSetViewport(cBuffer, 0, 1, &viewport);
    
    VkRect2D scissor{};
    scissor.offset =  { 0, 0 };
    scissor.extent = pPipeline->getSwapChain()->getSwapChainExtent();
    
    vkCmdSetScissor(cBuffer, 0, 1, &scissor);
    
    // binding buffers
    VkDeviceSize offsets[] = { 0 };
    
    vkCmdBindVertexBuffers(cBuffer, 0, 1, pMesh->getVertexBufferPointer(), offsets);
    vkCmdBindIndexBuffer(cBuffer, *(pMesh->getIndexBufferPointer()), 0, VK_INDEX_TYPE_UINT16);
    
    vkCmdBindDescriptorSets(cBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->getPipelineLayout(), 0, 1, pMesh->getDescriptorSet(currentFrame), 0, nullptr);
    
    vkCmdDrawIndexed(cBuffer, static_cast<uint32_t>(pMesh->getIndicesSize()), 1, 0, 0, 0);
    //vkCmdDraw(cBuffer, static_cast<uint32_t>(pMesh->getVerticesSize()), 1, 0, 0); // commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance
    vkCmdEndRenderPass(cBuffer);
    
    if (vkEndCommandBuffer(cBuffer) != VK_SUCCESS) {
        ConsoleText::printError("Failed to record command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was recorder!", "Renderer");
}

void Renderer::drawFrame(Pipeline* pPipeline, Mesh* pMesh)
{
    std::string message = "-------------------- Current frame: " + std::to_string(currentFrame) + " --------------------";
    ConsoleText::printGreen(message, "Renderer");
    
    
    vkWaitForFences(pAPImanager->getVulkanResources()->getLogicalDevice(), 1, &inFlightFences.at(currentFrame), VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(pAPImanager->getVulkanResources()->getLogicalDevice(),
                                            pPipeline->getSwapChain()->getSwapChain(),
                                            UINT64_MAX,
                                            imageAvailableSemaphores.at(currentFrame),
                                            VK_NULL_HANDLE,
                                            &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        ConsoleText::printPurple("The result of vkAcquireNextImageKHR() equals VK_ERROR_OUT_OF_DATE_KHR, swap chain was recreated!", "Renderer");
        pPipeline->getSwapChain()->recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        ConsoleText::printError("The result of vkAcquireNextImageKHR() is unsatisfactory!", "Renderer");
        std::exit(0);
    }
    
    pMesh->updateUniformBuffer(currentFrame, pPipeline->getSwapChain()->getSwapChainExtent());
    
    vkResetFences(pAPImanager->getVulkanResources()->getLogicalDevice(), 1, &inFlightFences.at(currentFrame));
    
    vkResetCommandBuffer(commandBuffers.at(currentFrame), 0);
    recordCommandBuffer(pPipeline,
                        pMesh,
                        commandBuffers.at(currentFrame),
                        pPipeline->getPipeline(),
                        imageIndex);
    
    // Command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers.at(currentFrame);
    
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores.at(currentFrame) };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores.at(currentFrame) };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    if (vkQueueSubmit(pAPImanager->getVulkanResources()->getGraphicsQueue(), 1, &submitInfo, inFlightFences.at(currentFrame)) != VK_SUCCESS) {
        ConsoleText::printError("Failed to submit draw command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Draw command buffer was submited!", "Renderer");
    
    // Presentation
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { pPipeline->getSwapChain()->getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(pAPImanager->getVulkanResources()->getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        pPipeline->getSwapChain()->recreateSwapChain();
        
        framebufferResized = false;
    }
    else if (result != VK_SUCCESS) {
        ConsoleText::printError("Failed to present swap chain image!", "Renderer");
        std::exit(0);
    }
    
    currentFrame = (currentFrame + 1) % max_frames_in_flight;
}

void Renderer::createSyncObjects() {
    imageAvailableSemaphores.resize(max_frames_in_flight);
    renderFinishedSemaphores.resize(max_frames_in_flight);
    inFlightFences.resize(max_frames_in_flight);
    
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        if (vkCreateSemaphore(pAPImanager->getVulkanResources()->getLogicalDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateSemaphore(pAPImanager->getVulkanResources()->getLogicalDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateFence(pAPImanager->getVulkanResources()->getLogicalDevice(), &fenceCreateInfo, nullptr, &inFlightFences.at(i)) != VK_SUCCESS) {
            ConsoleText::printError("Failed to create sync objects!", "Renderer");
            std::exit(0);
        }
        else ConsoleText::printGreen("Sync objects were created!", "Renderer");
    }
}

void Renderer::updateWindow() {
    ConsoleText::printGreen("-------------Framebuffer was resized!-------------", "Renderer");
    framebufferResized = true;
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(pAPImanager->getVulkanResources()->getLogicalDevice());
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        vkDestroySemaphore(pAPImanager->getVulkanResources()->getLogicalDevice(), imageAvailableSemaphores.at(i), nullptr);
        vkDestroySemaphore(pAPImanager->getVulkanResources()->getLogicalDevice(), renderFinishedSemaphores.at(i), nullptr);
        vkDestroyFence(pAPImanager->getVulkanResources()->getLogicalDevice(), inFlightFences.at(i), nullptr);
        ConsoleText::printGreen("Sync objects were destroyed!", "Renderer");
    }
    
    vkDestroyCommandPool(pAPImanager->getVulkanResources()->getLogicalDevice(), commandPool, nullptr);
    ConsoleText::printGreen("Command pool was destroyed!", "Renderer");
    
    pAPImanager = nullptr;
};

}

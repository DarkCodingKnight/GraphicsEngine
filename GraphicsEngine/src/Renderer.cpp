#include "Renderer.hpp"

namespace Engine {

Renderer::Renderer(VulkanResources* vulkanResources,
                   const int max_frames_in_flight,
                   GLFWwindow* const pWindow) :
pVulkanResources(vulkanResources)
{
    if (max_frames_in_flight == 0) std::exit(0);
    else this->max_frames_in_flight = max_frames_in_flight;
    
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void Renderer::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(pVulkanResources->getPhysicalDevice(),
                          pVulkanResources->getSurface());
    
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    if (vkCreateCommandPool(pVulkanResources->getLogicalDevice(), &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
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
    
    if (vkAllocateCommandBuffers(pVulkanResources->getLogicalDevice(), &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was allocated!", "Renderer");
}

void Renderer::recordCommandBuffer(std::vector<std::shared_ptr<GraphicsObjectPool>> pGraphicsObjectPools,
                                   SwapChain* pSwapChain,
                                   VkCommandBuffer& commandBuffer,
                                   uint32_t imageIndex)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
        ConsoleText::printError("Failed to begin command buffer!", "Renderer");
        return;
    }
    else ConsoleText::printGreen("Command buffer was begined!", "Renderer");
    
    
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = pSwapChain->getSwapChainRenderPass();
    renderPassBeginInfo.framebuffer = (pSwapChain->getSwapChainFramebuffers()).at(imageIndex);
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = pSwapChain->getSwapChainExtent();
    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };
    
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    
    
    for (std::shared_ptr<GraphicsObjectPool> pObjectPool : pGraphicsObjectPools)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pObjectPool->getPipeline()->getPipeline());
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(pSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(pSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        
        VkRect2D scissor{};
        scissor.offset =  { 0, 0 };
        scissor.extent = pSwapChain->getSwapChainExtent();
        
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        
        // binding buffers
        VkDeviceSize offsets[] = { 0 };
        
        for (std::shared_ptr<GraphicsObject> pObject : pObjectPool->getGraphicsObjects())
        {
            if (pObject->isDrawEnable()) {
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, pObject->getVertexBufferPointer(), offsets);
                //vkCmdBindIndexBuffer(commandBuffer, pObject->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
                
                vkCmdBindDescriptorSets(commandBuffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pObjectPool->getPipeline()->getPipelineLayout(),
                                        0, 1, pObject->getDescriptorSets(currentFrame),
                                        0, nullptr);
                
                //vkCmdDrawIndexed(commandBuffer, pObject->getIndexBufferSize(), 1, 0, 0, 0);
                
                vkCmdDraw(commandBuffer, pObject->getVertexBufferSize(), 1, 0, 0);
            }
        }
    }
    
    
    
    
    vkCmdEndRenderPass(commandBuffer);
    
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        ConsoleText::printError("Failed to record command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Command buffer was recorder!", "Renderer");
}

void Renderer::drawFrame(std::vector<std::shared_ptr<GraphicsObjectPool>> pGraphicsObjectPools,
                         SwapChain* pSwapChain)
{
    std::string message = "-------------------- Current frame: " + std::to_string(currentFrame) + " --------------------";
    ConsoleText::printGreen(message, "Renderer");
    
    
    vkWaitForFences(pVulkanResources->getLogicalDevice(), 1, &inFlightFences.at(currentFrame), VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(pVulkanResources->getLogicalDevice(),
                                            pSwapChain->getSwapChain(),
                                            UINT64_MAX,
                                            imageAvailableSemaphores.at(currentFrame),
                                            VK_NULL_HANDLE,
                                            &imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        ConsoleText::printPurple("The result of vkAcquireNextImageKHR() equals VK_ERROR_OUT_OF_DATE_KHR, swap chain was recreated!", "Renderer");
        pSwapChain->recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        ConsoleText::printError("The result of vkAcquireNextImageKHR() is unsatisfactory!", "Renderer");
        std::exit(0);
    }
    
    vkResetFences(pVulkanResources->getLogicalDevice(), 1, &inFlightFences.at(currentFrame));
    
    vkResetCommandBuffer(commandBuffers.at(currentFrame), 0);
    recordCommandBuffer(pGraphicsObjectPools, pSwapChain, commandBuffers[currentFrame], imageIndex);
    
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
    
    if (vkQueueSubmit(pVulkanResources->getGraphicsQueue(), 1, &submitInfo, inFlightFences.at(currentFrame)) != VK_SUCCESS) {
        ConsoleText::printError("Failed to submit draw command buffer!", "Renderer");
        std::exit(0);
    }
    else ConsoleText::printGreen("Draw command buffer was submited!", "Renderer");
    
    // Presentation
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { pSwapChain->getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(pVulkanResources->getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        pSwapChain->recreateSwapChain();
        
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
        if (vkCreateSemaphore(pVulkanResources->getLogicalDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateSemaphore(pVulkanResources->getLogicalDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateFence(pVulkanResources->getLogicalDevice(), &fenceCreateInfo, nullptr, &inFlightFences.at(i)) != VK_SUCCESS) {
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
    vkDeviceWaitIdle(pVulkanResources->getLogicalDevice());
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        vkDestroySemaphore(pVulkanResources->getLogicalDevice(), imageAvailableSemaphores.at(i), nullptr);
        vkDestroySemaphore(pVulkanResources->getLogicalDevice(), renderFinishedSemaphores.at(i), nullptr);
        vkDestroyFence(pVulkanResources->getLogicalDevice(), inFlightFences.at(i), nullptr);
        ConsoleText::printGreen("Sync objects were destroyed!", "Renderer");
    }
    
    vkDestroyCommandPool(pVulkanResources->getLogicalDevice(), commandPool, nullptr);
    ConsoleText::printGreen("Command pool was destroyed!", "Renderer");
    
    pVulkanResources = nullptr;
};

}

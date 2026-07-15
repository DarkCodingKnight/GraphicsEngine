#include "SwapChain.hpp"

namespace Engine {

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = getSwapChainSupport(pAPImanager->getVulkanResources()->getPhysicalDevice(),
                            pAPImanager->getVulkanResources()->getSurface());
    
    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR format = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    
    swapChainImageFormat = surfaceFormat.format;
    swapChainImageExtent = extent;
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = pAPImanager->getVulkanResources()->getSurface();
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = format;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    
    QueueFamilyIndices indices = findQueueFamilies(pAPImanager->getVulkanResources()->getPhysicalDevice(),
                                                   pAPImanager->getVulkanResources()->getSurface());
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    
    if (indices.graphicsFamily != indices.presentFamily) {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }
    
    if (vkCreateSwapchainKHR(pAPImanager->getVulkanResources()->getLogicalDevice(), &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create swap chain!", "SwapChain");
        std::exit(0);
    }
    else ConsoleText::printGreen("Swap chain was created!", "SwapChain");
    
    getSwapChainImages(&imageCount);
}

void SwapChain::getSwapChainImages(uint32_t* imageCount) {
    vkGetSwapchainImagesKHR(pAPImanager->getVulkanResources()->getLogicalDevice(), swapChain, imageCount, nullptr);
    swapChainImages.resize(*imageCount);
    vkGetSwapchainImagesKHR(pAPImanager->getVulkanResources()->getLogicalDevice(), swapChain, imageCount, swapChainImages.data());
}

VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats.at(0);
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(pGLFWwindow, &width, &height);
        
        VkExtent2D actualExtent{};
        actualExtent.width = std::clamp(static_cast<uint32_t>(width),
                                        capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(static_cast<uint32_t>(height),
                                        capabilities.minImageExtent.height,
                                        capabilities.maxImageExtent.height);
        
        return actualExtent;
    }
}

void SwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());
    
    for (int i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(pAPImanager->getVulkanResources()->getLogicalDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            std::string message = "Failed to create image view " + std::to_string(i + 1) + "!";
            ConsoleText::printError(message, "Swap chain");
            std::exit(0);
        }
        else {
            std::string message = "Image view " + std::to_string(i + 1) + " was created!";
            ConsoleText::printGreen(message, "Swap chain");
        }
    }
}

void SwapChain::createRenderPass() {
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
    
    if (vkCreateRenderPass(pAPImanager->getVulkanResources()->getLogicalDevice(), &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create render pass", "Renderer");
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Render pass was created!", "Renderer");
    }
}

void SwapChain::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());
    
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &(swapChainImageViews.at(i));
        framebufferCreateInfo.width = swapChainImageExtent.width;
        framebufferCreateInfo.height = swapChainImageExtent.height;
        framebufferCreateInfo.layers = 1;
        
        if (vkCreateFramebuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), &framebufferCreateInfo, nullptr, &(swapChainFramebuffers.at(i))) != VK_SUCCESS) {
            std::string message = "Failed to create framebuffer " + std::to_string(i + 1) + ".";
            ConsoleText::printError(message, "Renderer");
        }
        else {
            std::string message = "Framebuffer " + std::to_string(i + 1) + " was created.";
            ConsoleText::printGreen(message, "Renderer");
        }
    }
}

void SwapChain::recreateSwapChain() {
    int width = 0;
    int height = 0;
    
    glfwGetFramebufferSize(pGLFWwindow, &width, &height);
    
    while (width == 0 || height == 0) {
        std::cout << "wegfkwegjierhgierbergvelirygvelgawegfkwegjierhgierbergvelirygvelga" << std::endl;
        glfwGetFramebufferSize(pGLFWwindow, &width, &height);
        glfwPollEvents();
    }
    
    vkDeviceWaitIdle(pAPImanager->getVulkanResources()->getLogicalDevice());
    
    cleanUp();
    
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
}

void SwapChain::cleanUp() {
    for (int i = 0; i < swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), swapChainFramebuffers.at(i), nullptr);
        std::string message = "Framebuffer " + std::to_string(i + 1) + " was destroyed.";
        ConsoleText::printGreen(message, "Renderer");
    }
    
    vkDestroyRenderPass(pAPImanager->getVulkanResources()->getLogicalDevice(), renderPass, nullptr);
    
    for (int i = 0; i < swapChainImageViews.size(); i++) {
        vkDestroyImageView(pAPImanager->getVulkanResources()->getLogicalDevice(), swapChainImageViews.at(i), nullptr);
        std::string message = "Image view " + std::to_string(i + 1) + " was destroyed!";
        ConsoleText::printGreen(message, "Swap chain");
    }
    
    vkDestroySwapchainKHR(pAPImanager->getVulkanResources()->getLogicalDevice(), swapChain, nullptr);
    ConsoleText::printGreen("Swap chain was destroyed!", "SwapChain");
}

SwapChain::~SwapChain() {
    cleanUp();
    
    pAPImanager = nullptr;
    pGLFWwindow = nullptr;
};

}

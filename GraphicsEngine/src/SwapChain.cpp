#include "SwapChain.hpp"

namespace Engine {

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = getSwapChainSupport(pVulkanResources->getPhysicalDevice(),
                            pVulkanResources->getSurface());
    
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
    swapChainCreateInfo.surface = pVulkanResources->getSurface();
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
    
    QueueFamilyIndices indices = findQueueFamilies(pVulkanResources->getPhysicalDevice(),
                          pVulkanResources->getSurface());
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
    
    if (vkCreateSwapchainKHR(pVulkanResources->getLogicalDevice(), &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create swap chain!", "SwapChain");
        std::exit(0);
    }
    else ConsoleText::printGreen("Swap chain was created!", "SwapChain");
    
    createSwapChainImages(&imageCount);
}

void SwapChain::createSwapChainImages(uint32_t* imageCount) {
    vkGetSwapchainImagesKHR(pVulkanResources->getLogicalDevice(), swapChain, imageCount, nullptr);
    swapChainImages.resize(*imageCount);
    vkGetSwapchainImagesKHR(pVulkanResources->getLogicalDevice(), swapChain, imageCount, swapChainImages.data());
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
        createImageView(pVulkanResources->getLogicalDevice(),
                        &swapChainImageViews[i],
                        swapChainImages[i],
                        swapChainImageFormat,
                        VK_IMAGE_ASPECT_COLOR_BIT,
                        1);
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
    
    // Depth image
    
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat(pVulkanResources->getPhysicalDevice());
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference depthAttachmentReference{};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    
    std::vector<VkAttachmentDescription> attachments = {
        colorAttachment,
        depthAttachment
    };
    
    
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    
    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
   
    
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;
    
    if (vkCreateRenderPass(pVulkanResources->getLogicalDevice(), &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
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
        std::vector<VkImageView> attachments = {
            swapChainImageViews[i],
            depthImageView
        };
        
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = swapChainImageExtent.width;
        framebufferCreateInfo.height = swapChainImageExtent.height;
        framebufferCreateInfo.layers = 1;
        
        if (vkCreateFramebuffer(pVulkanResources->getLogicalDevice(), &framebufferCreateInfo, nullptr, &(swapChainFramebuffers.at(i))) != VK_SUCCESS) {
            std::string message = "Failed to create framebuffer " + std::to_string(i + 1) + ".";
            ConsoleText::printError(message, "Renderer");
        }
        else {
            std::string message = "Framebuffer " + std::to_string(i + 1) + " was created.";
            ConsoleText::printGreen(message, "Renderer");
        }
    }
}

void SwapChain::createDepthResources()
{
    const std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    
    VkFormat depthFormat = findSupportedFormat(pVulkanResources->getPhysicalDevice(),
                                               candidates,
                                               VK_IMAGE_TILING_OPTIMAL,
                                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    
    createImage(pVulkanResources->getLogicalDevice(),
                pVulkanResources->getPhysicalDevice(),
                depthImage,
                depthImageMemory,
                swapChainImageExtent.width,
                swapChainImageExtent.height,
                1,
                depthFormat,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    createImageView(pVulkanResources->getLogicalDevice(),
                    &depthImageView,
                    depthImage,
                    depthFormat,
                    VK_IMAGE_ASPECT_DEPTH_BIT,
                    1);
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
    
    vkDeviceWaitIdle(pVulkanResources->getLogicalDevice());
    
    cleanUp();
    
    createSwapChain();
    createImageViews();
    createDepthResources();
    createRenderPass();
    createFramebuffers();
}

void SwapChain::cleanUp()
{
    for (int i = 0; i < swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(pVulkanResources->getLogicalDevice(), swapChainFramebuffers.at(i), nullptr);
        std::string message = "Framebuffer " + std::to_string(i + 1) + " was destroyed.";
        ConsoleText::printGreen(message, "Renderer");
    }
    
    vkDestroyRenderPass(pVulkanResources->getLogicalDevice(), renderPass, nullptr);
    
    vkDestroyImageView(pVulkanResources->getLogicalDevice(), depthImageView, nullptr);
    vkDestroyImage(pVulkanResources->getLogicalDevice(), depthImage, nullptr);
    vkFreeMemory(pVulkanResources->getLogicalDevice(), depthImageMemory, nullptr);
    
    for (int i = 0; i < swapChainImageViews.size(); i++) {
        vkDestroyImageView(pVulkanResources->getLogicalDevice(), swapChainImageViews.at(i), nullptr);
        std::string message = "Image view " + std::to_string(i + 1) + " was destroyed!";
        ConsoleText::printGreen(message, "Swap chain");
    }
    
    vkDestroySwapchainKHR(pVulkanResources->getLogicalDevice(), swapChain, nullptr);
    ConsoleText::printGreen("Swap chain was destroyed!", "SwapChain");
}

SwapChain::~SwapChain() {
    cleanUp();
    
    pVulkanResources = nullptr;
    pGLFWwindow = nullptr;
};

}

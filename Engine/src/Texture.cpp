#include "Texture.hpp"

namespace Engine {

Texture::Texture(APImanager* pManager,
                 const char* texPath,
                 const VkCommandPool& commandPool) : pAPImanager(pManager)
{
    createTextureImage(texPath, commandPool);
    
    createImageView(pAPImanager->getVulkanResources()->getLogicalDevice(),
                    &textureImageView,
                    textureImage,
                    VK_FORMAT_R8G8B8A8_SRGB);
    
    createTextureSampler();
}

void Texture::createTextureImage(const char* texPath, const VkCommandPool& commandPool) {
    TextureData texData = ResourceManager::loadTexture(texPath);
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    createBufferSource(pAPImanager->getVulkanResources()->getLogicalDevice(),
                       pAPImanager->getVulkanResources()->getPhysicalDevice(),
                       texData.texSize,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer,
                       stagingBufferMemory);
    
    void* data;
    vkMapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBufferMemory, 0, texData.texSize, 0, &data);
    memcpy(data, texData.pixelsData, texData.texSize);
    vkUnmapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBufferMemory);
    
    createImage(texData.texWidth,
                texData.texHeight,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                textureImage,
                textureImageMemory);
    
    transitionImageLayout(textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          commandPool);
    
    copyBufferToImage(stagingBuffer,
                      textureImage,
                      static_cast<uint32_t>(texData.texWidth),
                      static_cast<uint32_t>(texData.texHeight),
                      commandPool);
    
    transitionImageLayout(textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          commandPool);
    
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void Texture::createImage(uint32_t width,
                          uint32_t height,
                          VkFormat format,
                          VkImageTiling tiling,
                          VkImageUsageFlags usage,
                          VkMemoryPropertyFlags properties,
                          VkImage& image,
                          VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateImage(pAPImanager->getVulkanResources()->getLogicalDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create texture!", "Texture");
        std::exit(0);
    }
    else ConsoleText::printGreen("Texture was created!", "Texture");
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(pAPImanager->getVulkanResources()->getLogicalDevice(), textureImage, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(pAPImanager->getVulkanResources()->getPhysicalDevice(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    if (vkAllocateMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate image memory!", "Texture");
        std::exit(0);
    }
    
    vkBindImageMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), textureImage, textureImageMemory, 0);
}

VkCommandBuffer Texture::beginSingleTimeCommands(const VkCommandPool& commandPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(pAPImanager->getVulkanResources()->getLogicalDevice(),
                             &allocInfo,
                             &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    return commandBuffer;
}

void Texture::endSingleTimeCommands(VkCommandBuffer commandBuffer,
                                    const VkCommandPool& commandPool)
{
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkQueueSubmit(pAPImanager->getVulkanResources()->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(pAPImanager->getVulkanResources()->getGraphicsQueue());
    
    vkFreeCommandBuffers(pAPImanager->getVulkanResources()->getLogicalDevice(),
                         commandPool,
                         1,
                         &commandBuffer);
}

void Texture::copyBufferToImage(VkBuffer buffer,
                                VkImage image,
                                uint32_t width,
                                uint32_t height,
                                const VkCommandPool& commandPool)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);
    
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    
    vkCmdCopyBufferToImage(commandBuffer,
                           buffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region);
    
    endSingleTimeCommands(commandBuffer, commandPool);
}

void Texture::transitionImageLayout(VkImage image,
                                    VkFormat format,
                                    VkImageLayout oldLayout,
                                    VkImageLayout newLayout,
                                    const VkCommandPool& commandPool)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);
    
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        ConsoleText::printError("Unsupported layout transition!", "Texture");
        std::exit(0);
    }
    
    vkCmdPipelineBarrier(commandBuffer,
                         sourceStage,
                         destinationStage,
                         0, 0,
                         nullptr, 0,
                         nullptr, 1, &barrier);
    
    endSingleTimeCommands(commandBuffer, commandPool);
}

void Texture::createTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(pAPImanager->getVulkanResources()->getPhysicalDevice(), &deviceProperties);
    
    samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    
    if (vkCreateSampler(pAPImanager->getVulkanResources()->getLogicalDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create texture sampler!", "Texture");
        std::exit(0);
    }
    else ConsoleText::printGreen("Texture sampler was created!", "Texture");
}

Texture::~Texture() {
    vkDestroySampler(pAPImanager->getVulkanResources()->getLogicalDevice(), textureSampler, nullptr);
    
    vkDestroyImageView(pAPImanager->getVulkanResources()->getLogicalDevice(), textureImageView, nullptr);
    
    vkDestroyImage(pAPImanager->getVulkanResources()->getLogicalDevice(), textureImage, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), textureImageMemory, nullptr);
    
    pAPImanager = nullptr;
}

}

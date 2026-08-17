#include "Texture.hpp"

namespace Engine {

Texture::Texture(VulkanResources* vulkanResources,
                 const char* texPath,
                 const VkCommandPool& commandPool) :
pVulkanResources(vulkanResources)
{
    createTextureImage(texPath, commandPool);
    
    createImageView(pVulkanResources->getLogicalDevice(),
                    &textureImageView,
                    textureImage,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    mipLevels);
}

void Texture::createTextureImage(const char* texPath,
                                 const VkCommandPool& commandPool)
{
    if(texPath != nullptr) {
        textureData = {};
        ResourceManager::loadTexture(texPath, &textureData);
        
        if (textureData.mipLevels != 0) mipLevels = textureData.mipLevels;
    }
    else ConsoleText::printError("Texture path is nullptr!", "Texture");
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    createBufferSource(pVulkanResources->getLogicalDevice(),
                       pVulkanResources->getPhysicalDevice(),
                       textureData.texSize,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer,
                       stagingBufferMemory);
    
    void* data;
    vkMapMemory(pVulkanResources->getLogicalDevice(), stagingBufferMemory, 0, textureData.texSize, 0, &data);
    memcpy(data, textureData.pixelsData, textureData.texSize);
    vkUnmapMemory(pVulkanResources->getLogicalDevice(), stagingBufferMemory);
    
    createImage(pVulkanResources->getLogicalDevice(),
                pVulkanResources->getPhysicalDevice(),
                textureImage,
                textureImageMemory,
                textureData.texWidth,
                textureData.texHeight,
                mipLevels,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    transitionImageLayout(pVulkanResources->getLogicalDevice(),
                          pVulkanResources->getGraphicsQueue(),
                          textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          commandPool,
                          mipLevels);
    
    copyBufferToImage(stagingBuffer,
                      textureImage,
                      static_cast<uint32_t>(textureData.texWidth),
                      static_cast<uint32_t>(textureData.texHeight),
                      commandPool);
    
    
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(pVulkanResources->getPhysicalDevice(),
                                        VK_FORMAT_R8G8B8A8_SRGB,
                                        &formatProperties);
    
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        ConsoleText::printError("Texture image format does not support linear blitting!");
        
        transitionImageLayout(pVulkanResources->getLogicalDevice(),
                              pVulkanResources->getGraphicsQueue(),
                              textureImage,
                              VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              commandPool,
                              mipLevels);
    }
    else {
        generateMipmaps(commandPool,
                        textureImage,
                        textureData.texWidth,
                        textureData.texHeight,
                        mipLevels);
    }
    
    vkDestroyBuffer(pVulkanResources->getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(pVulkanResources->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void Texture::copyBufferToImage(VkBuffer buffer,
                                VkImage image,
                                uint32_t width,
                                uint32_t height,
                                const VkCommandPool& commandPool)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pVulkanResources->getLogicalDevice(), commandPool);
    
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
    
    endSingleTimeCommands(pVulkanResources->getLogicalDevice(),
                          pVulkanResources->getGraphicsQueue(),
                          commandBuffer,
                          commandPool);
}

void Texture::generateMipmaps(const VkCommandPool& commandPool,
                              VkImage image,
                              int32_t texWidth,
                              int32_t texHeight,
                              uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pVulkanResources->getLogicalDevice(),
                                                            commandPool);
    
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;
    
    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;
    
    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
        
        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        
        vkCmdBlitImage(commandBuffer,
                       image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);
        
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
        
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }
    
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    
    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
    
    endSingleTimeCommands(pVulkanResources->getLogicalDevice(),
                          pVulkanResources->getGraphicsQueue(),
                          commandBuffer,
                          commandPool);
}

Texture::~Texture() {
    vkDestroyImageView(pVulkanResources->getLogicalDevice(), textureImageView, nullptr);
    
    vkDestroyImage(pVulkanResources->getLogicalDevice(), textureImage, nullptr);
    vkFreeMemory(pVulkanResources->getLogicalDevice(), textureImageMemory, nullptr);
    
    pVulkanResources = nullptr;
}

}

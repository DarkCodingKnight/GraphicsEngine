#pragma once

#include "APImanager.hpp"
#include "VulkanData.hpp"
#include "ResourceManager.hpp"

namespace Engine{

class Texture {
public:
    Texture() = delete;
    Texture(APImanager* pManager,
            const char* texPath,
            const VkCommandPool& commandPool);
    
    VkSampler& getImageSampler() { return textureSampler; };
    VkImageView& getImageView() { return textureImageView; };
    
    ~Texture();
    
private:
    void createTextureImage(const char* texPath, const VkCommandPool& commandPool);
    void createImage(uint32_t width,
                     uint32_t height,
                     VkFormat format,
                     VkImageTiling tiling,
                     VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkImage& image,
                     VkDeviceMemory& imageMemory);
    VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer,
                               const VkCommandPool& commandPool);
    void copyBufferToImage(VkBuffer buffer,
                           VkImage image,
                           uint32_t width,
                           uint32_t height,
                           const VkCommandPool& commandPool);
    void transitionImageLayout(VkImage image,
                               VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout,
                               const VkCommandPool& commandPool);
    
    void createTextureSampler();
    
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    
    APImanager* pAPImanager = nullptr;
};

}

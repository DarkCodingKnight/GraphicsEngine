#pragma once

#include "ResourceManager.hpp"
#include "VulkanResources.hpp"

namespace Engine{

class Texture {
public:
    Texture() = delete;
    Texture(VulkanResources* vulkanResources,
            const char* texPath,
            const VkCommandPool& commandPool);
    
    VkImageView& getImageView() { return textureImageView; };
    
    ~Texture();
    
private:
    void createTextureImage(const char* texPath, const VkCommandPool& commandPool);
    void copyBufferToImage(VkBuffer buffer,
                           VkImage image,
                           uint32_t width,
                           uint32_t height,
                           const VkCommandPool& commandPool);
    
    void generateMipmaps(const VkCommandPool& commandPool,
                         VkImage image,
                         int32_t texWidth,
                         int32_t texHeight,
                         uint32_t mipLevels);
    
    uint32_t mipLevels = 1;
    
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    
    TextureData textureData;
    
    VulkanResources* pVulkanResources = nullptr;
};

}

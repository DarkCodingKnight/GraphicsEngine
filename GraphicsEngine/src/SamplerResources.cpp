#include "SamplerResources.hpp"

namespace Engine {

Sampler::Sampler(VulkanResources* _pVulkanResources_,
        SwapChain* _pSwapChain_) :
pVulkanResources(_pVulkanResources_), pSwapChain(_pSwapChain_)
{
    createTextureSampler();
}

void Sampler::createTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(pVulkanResources->getPhysicalDevice(), &deviceProperties);
    
    samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    
    if (vkCreateSampler(pVulkanResources->getLogicalDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create texture sampler!", "Texture");
        std::exit(0);
    }
    else ConsoleText::printGreen("Texture sampler was created!", "Texture");
}

void Sampler::createColorResources() {
    msaaSamples = getMaxUsableSampleCount();
    
    createImage(pVulkanResources->getLogicalDevice(),
                pVulkanResources->getPhysicalDevice(),
                colorImage,
                colorImageMemory,
                pSwapChain->getSwapChainExtent().width,
                pSwapChain->getSwapChainExtent().height,
                1, pSwapChain->getSwapChainImageFormat(),
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                msaaSamples);
}

VkSampleCountFlagBits Sampler::getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(pVulkanResources->getPhysicalDevice(), &physicalDeviceProperties);
    
    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    
    if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
    else if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
    else if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
    else if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
    else if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
    else if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;
    else return VK_SAMPLE_COUNT_1_BIT;
}

Sampler::~Sampler() {
    pSwapChain = nullptr;
    pVulkanResources = nullptr;
    
    vkDestroySampler(pVulkanResources->getLogicalDevice(), textureSampler, nullptr);
    ConsoleText::printGreen("Texture sampler was destroyed!", "ObjectPool");
}

}

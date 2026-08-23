#pragma once

#include "VulkanResources.hpp"
#include "SwapChain.hpp"

namespace Engine {

class Sampler {
public:
    Sampler() = delete;
    Sampler(const Sampler& other) = delete;
    Sampler(Sampler&& other) = delete;
    Sampler& operator=(const Sampler& other) = delete;
    Sampler& operator=(Sampler&& other) = delete;
    
    Sampler(VulkanResources* _pVulkanResources_,
            SwapChain* _pSwapChain_);
    
    void createColorResources();
    
    VkSampleCountFlagBits getSampleFlagBits() { return msaaSamples; };
    VkSampler& getSampler() { return textureSampler; };
    
    ~Sampler();
    
private:
    void createTextureSampler();
    VkSampleCountFlagBits getMaxUsableSampleCount();
    
    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
    
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkSampler textureSampler;
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
};

}

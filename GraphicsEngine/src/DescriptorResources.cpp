#include "DescriptorResources.hpp"

namespace Engine {

IMGUIDescriptorPool::IMGUIDescriptorPool(VulkanResources* _pVulkanResources_,
                                         uint16_t _max_frames_in_flight_) :
pVulkanResources(_pVulkanResources_)
{
    createDescriptorPool(_pVulkanResources_, _max_frames_in_flight_);
}

void IMGUIDescriptorPool::createDescriptorPool(VulkanResources* _pVulkanResources_,
                                               uint16_t _max_frames_in_flight_)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(poolSizes.size() * 11);
    
    if (vkCreateDescriptorPool(_pVulkanResources_->getLogicalDevice(),
                               &poolInfo,
                               nullptr,
                               &descriptorPool) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create IMGUI descriptor pool!", "Pipeline");
        std::exit(0);
    }
    else { ConsoleText::printGreen("IMGUI descriptor pool was created!", "Pipeline"); }
}

IMGUIDescriptorPool::~IMGUIDescriptorPool() {
    vkDestroyDescriptorPool(pVulkanResources->getLogicalDevice(), descriptorPool, nullptr);
    ConsoleText::printGreen("IMGUI descriptor pool was destroyed!", "ObjectPool");
    
    pVulkanResources = nullptr;
}


EngineDescriptorPool::EngineDescriptorPool(VulkanResources* _pVulkanResources_,
                                           uint16_t _max_frames_in_flight_) :
pVulkanResources(_pVulkanResources_)
{
    createDescriptorPool(_pVulkanResources_, _max_frames_in_flight_);
}

void EngineDescriptorPool::createDescriptorPool(VulkanResources* _pVulkanResources_,
                                                uint16_t _max_frames_in_flight_) {
    uint32_t size = 100;
    
    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, size },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, size }
    };
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = size;
    
    if (vkCreateDescriptorPool(_pVulkanResources_->getLogicalDevice(),
                               &poolInfo,
                               nullptr,
                               &descriptorPool) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create engine descriptor pool!", "Pipeline");
        std::exit(0);
    }
    else { ConsoleText::printGreen("Engine descriptor pool was created!", "Pipeline"); }
}

EngineDescriptorPool::~EngineDescriptorPool() {
    vkDestroyDescriptorPool(pVulkanResources->getLogicalDevice(), descriptorPool, nullptr);
    ConsoleText::printGreen("Engine Descriptor pool was destroyed!", "ObjectPool");
    
    pVulkanResources = nullptr;
}


DescriptorResources::DescriptorResources(VulkanResources* _pVulkanResources_,
                                         uint16_t _max_frames_in_flight_) :
pVulkanResources(_pVulkanResources_), max_frames_in_flight(_max_frames_in_flight_)
{
    createDescriptorSetLayout();
}

void DescriptorResources::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
    
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorLayoutInfo.pBindings = bindings.data();
    
    if (vkCreateDescriptorSetLayout(pVulkanResources->getLogicalDevice(), &descriptorLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create descriptor set layout!", "Pipeline");
        std::exit(0);
    }
    else { ConsoleText::printGreen("Descriptor set layout was created!", "Pipeline"); }
}

DescriptorResources::~DescriptorResources() {
    vkDestroyDescriptorSetLayout(pVulkanResources->getLogicalDevice(), descriptorSetLayout, nullptr);
    ConsoleText::printGreen("Descriptor set layout was destroyed!", "ObjectPool");
}

}

#include "GraphicsObjectPool.hpp"

namespace Engine {

GraphicsObjectPool::GraphicsObjectPool(VulkanResources* vulkanResources,
                                       SwapChain* swapChain,
                                       VkCommandPool& comPool,
                                       std::string vertexPath,
                                       std::string fragmentPath,
                                       const GLFWwindow* pGLFWwindow,
                                       uint16_t max_frames) :
pVulkanResources(vulkanResources), max_frames_in_flight(max_frames), commandPool(comPool)
{
    std::vector<char> vertCode{};
    std::vector<char> fragCode{};
    
    std::optional<std::vector<char>> vertEnableCode = ResourceManager::readFile(vertexPath);
    if (vertEnableCode.has_value()) vertCode = vertEnableCode.value();
    
    std::optional<std::vector<char>> fragEnableCode = ResourceManager::readFile(fragmentPath);
    if (fragEnableCode.has_value()) fragCode = fragEnableCode.value();
    
    createDescriptorSetLayout();
    
    pPipeline = new Pipeline(pVulkanResources,
                             swapChain,
                             &descriptorSetLayout,
                             pGLFWwindow,
                             vertCode,
                             fragCode,
                             max_frames_in_flight);
    
    createDescriptorPool();
    createTextureSampler();
}

std::weak_ptr<GraphicsObject> GraphicsObjectPool::createGraphicsObject(const char* modelPath,
                                                                       const char* texturePath)
{
    std::shared_ptr<GraphicsObject> pGraphicsObject =
    std::make_shared<GraphicsObject>(pVulkanResources,
                                     commandPool,
                                     descriptorSetLayout,
                                     descriptorPool,
                                     textureSampler,
                                     texturePath,
                                     modelPath,
                                     max_frames_in_flight);
    
    pGraphicsObjects.push_back(pGraphicsObject);
    
    return pGraphicsObject;
}

void GraphicsObjectPool::createDescriptorSetLayout() {
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
    else ConsoleText::printGreen("Descriptor set layout was created!", "Pipeline");
}

void GraphicsObjectPool::createDescriptorPool() {
    VkDescriptorPoolSize uniformBufferDescriptorPoolSize{};
    uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(max_frames_in_flight);
    
    VkDescriptorPoolSize imageSamplerDescriptorPoolSize{};
    imageSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageSamplerDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(max_frames_in_flight);
    
    std::vector<VkDescriptorPoolSize> poolSizes = { uniformBufferDescriptorPoolSize, imageSamplerDescriptorPoolSize };
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(max_frames_in_flight);
    
    if (vkCreateDescriptorPool(pVulkanResources->getLogicalDevice(),
                               &poolInfo,
                               nullptr,
                               &descriptorPool) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create descriptor pool!", "Pipeline");
        std::exit(0);
    }
    else ConsoleText::printGreen("Descriptor pool was created!", "Pipeline");
}

void GraphicsObjectPool::createTextureSampler() {
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

GraphicsObjectPool::~GraphicsObjectPool() {
    vkDestroyDescriptorSetLayout(pVulkanResources->getLogicalDevice(), descriptorSetLayout, nullptr);
    ConsoleText::printGreen("Descriptor set layout was destroyed!", "ObjectPool");
    
    vkDestroyDescriptorPool(pVulkanResources->getLogicalDevice(), descriptorPool, nullptr);
    ConsoleText::printGreen("Descriptor pool was destroyed!", "ObjectPool");
    
    vkDestroySampler(pVulkanResources->getLogicalDevice(), textureSampler, nullptr);
    ConsoleText::printGreen("Texture sampler was destroyed!", "ObjectPool");
    
    delete pPipeline;
    
    pPipeline = nullptr;
    pVulkanResources = nullptr;
}

}

#include "Mesh.hpp"

namespace Engine {

Mesh::Mesh(APImanager* pManager,
           const VkCommandPool& commandPool,
           const std::vector<Vertex>& vert,
           const std::vector<uint16_t>& ind,
           const int _frames_,
           const VkDescriptorSetLayout& descriptorSetLayout) :
pAPImanager(pManager), vertices(vert), indices(ind)
{
    if (_frames_ == 0) {
        ConsoleText::printError("Max frames on flight equals 0", "Mesh");
        std::exit(0);
    }
    else max_frames_in_flight = _frames_;
    
    VkDeviceSize vertexBufferSize = sizeof(vertices.at(0)) * vertices.size();
    createBuffer(vertices.data(), vertexBufferSize, commandPool, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexBufferMemory);
    
    VkDeviceSize indexBufferSize = sizeof(indices.at(0)) * indices.size();
    createBuffer(indices.data(), indexBufferSize, commandPool, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexBufferMemory);
    
    createUniformBuffers(max_frames_in_flight);
    
    createDescriptorPool();
    createDescriptorSets(descriptorSetLayout);
};

template<typename T>
void Mesh::createBuffer(const T* dataArray,
                        const VkDeviceSize bufferSize,
                        const VkCommandPool commandPool,
                        VkBufferUsageFlagBits bufferUsageBit,
                        VkBuffer& dstBuffer,
                        VkDeviceMemory& dstMemory)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    createBufferSource(pAPImanager->getVulkanResources()->getLogicalDevice(),
                       pAPImanager->getVulkanResources()->getPhysicalDevice(),
                       bufferSize,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer,
                       stagingBufferMemory);
    
    void* data;
    vkMapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, dataArray, (size_t)bufferSize);
    vkUnmapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBufferMemory);
    
    createBufferSource(pAPImanager->getVulkanResources()->getLogicalDevice(),
                       pAPImanager->getVulkanResources()->getPhysicalDevice(),
                       bufferSize,
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageBit,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       dstBuffer,
                       dstMemory);
    
    copyBuffer(stagingBuffer, dstBuffer, bufferSize, commandPool);
    
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void Mesh::copyBuffer(VkBuffer srcBuffer,
                      VkBuffer dstBuffer,
                      VkDeviceSize size,
                      const VkCommandPool& commandPool)
{
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
    
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkQueueSubmit(pAPImanager->getVulkanResources()->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(pAPImanager->getVulkanResources()->getGraphicsQueue());
    
    vkFreeCommandBuffers(pAPImanager->getVulkanResources()->getLogicalDevice(), commandPool, 1, &commandBuffer);
}

void Mesh::createUniformBuffers(int max_frames_in_flight) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    
    uniformBuffers.resize(max_frames_in_flight);
    uniformBuffersMemory.resize(max_frames_in_flight);
    uniformBuffersMapped.resize(max_frames_in_flight);
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        createBufferSource(pAPImanager->getVulkanResources()->getLogicalDevice(),
                           pAPImanager->getVulkanResources()->getPhysicalDevice(),
                           bufferSize,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           uniformBuffers[i],
                           uniformBuffersMemory[i]);
        
        vkMapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(),
                    uniformBuffersMemory[i],
                    0,
                    bufferSize,
                    0,
                    &uniformBuffersMapped[i]);
    }
}

void Mesh::updateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.project = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);
    ubo.project[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Mesh::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(max_frames_in_flight);
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(max_frames_in_flight);
    
    if (vkCreateDescriptorPool(pAPImanager->getVulkanResources()->getLogicalDevice(),
                               &poolInfo,
                               nullptr,
                               &descriptorPool) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create descriptor pool!", "Pipeline");
        std::exit(0);
    }
    else ConsoleText::printGreen("Descriptor pool was created!", "Pipeline");
}

void Mesh::createDescriptorSets(const VkDescriptorSetLayout& descriptorSetLayout) {
    std::vector<VkDescriptorSetLayout> layouts(max_frames_in_flight, descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(max_frames_in_flight);
    allocInfo.pSetLayouts = layouts.data();
    
    descriptorSets.resize(max_frames_in_flight);
    
    if (vkAllocateDescriptorSets(pAPImanager->getVulkanResources()->getLogicalDevice(),
                                 &allocInfo,
                                 descriptorSets.data()) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate descriptor sets!", "Pipeline");
        std::exit(0);
    }
    else ConsoleText::printGreen("Descriptor sets were allocated!", "Pipeline");
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers.at(i);
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);
        
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        
        vkUpdateDescriptorSets(pAPImanager->getVulkanResources()->getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

Mesh::~Mesh() {
    vkDestroyDescriptorPool(pAPImanager->getVulkanResources()->getLogicalDevice(), descriptorPool, nullptr);
    ConsoleText::printGreen("Descriptor pool was destroyed!", "Descriptors");
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), uniformBuffersMemory[i], nullptr);
    }
    
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), indexBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), indexBufferMemory, nullptr);
    
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBufferMemory, nullptr);
    
    pAPImanager = nullptr;
};

}

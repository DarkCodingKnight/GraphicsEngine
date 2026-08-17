#include "Mesh.hpp"

namespace Engine {

Mesh::Mesh(VulkanResources* vulkanResources,
           const char* modelPath,
           const VkCommandPool& commandPool,
           const int _frames_) :
pVulkanResources(vulkanResources), max_frames_in_flight(_frames_)
{
    if (modelPath != nullptr) {
        ResourceManager::loadModel(modelPath, &vertices, &indices);
    }
    else ConsoleText::printError("Model path is nullptr!", "Mesh");
    
    VkDeviceSize vertexBufferSize = sizeof(vertices.at(0)) * vertices.size();
    createBuffer(vertices.data(), vertexBufferSize, commandPool, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexBufferMemory);
    
    VkDeviceSize indexBufferSize = sizeof(indices.at(0)) * indices.size();
    createBuffer(indices.data(), indexBufferSize, commandPool, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexBufferMemory);
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
    
    createBufferSource(pVulkanResources->getLogicalDevice(),
                       pVulkanResources->getPhysicalDevice(),
                       bufferSize,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer,
                       stagingBufferMemory);
    
    void* data;
    vkMapMemory(pVulkanResources->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, dataArray, (size_t)bufferSize);
    vkUnmapMemory(pVulkanResources->getLogicalDevice(), stagingBufferMemory);
    
    createBufferSource(pVulkanResources->getLogicalDevice(),
                       pVulkanResources->getPhysicalDevice(),
                       bufferSize,
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageBit,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       dstBuffer,
                       dstMemory);
    
    copyBuffer(stagingBuffer, dstBuffer, bufferSize, commandPool);
    
    vkDestroyBuffer(pVulkanResources->getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(pVulkanResources->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void Mesh::copyBuffer(VkBuffer srcBuffer,
                      VkBuffer dstBuffer,
                      VkDeviceSize size,
                      const VkCommandPool& commandPool)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(pVulkanResources->getLogicalDevice(),
                                                            commandPool);
    
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    endSingleTimeCommands(pVulkanResources->getLogicalDevice(),
                          pVulkanResources->getGraphicsQueue(),
                          commandBuffer,
                          commandPool);
}

Mesh::~Mesh() {
    vkDestroyBuffer(pVulkanResources->getLogicalDevice(), indexBuffer, nullptr);
    vkFreeMemory(pVulkanResources->getLogicalDevice(), indexBufferMemory, nullptr);
    
    vkDestroyBuffer(pVulkanResources->getLogicalDevice(), vertexBuffer, nullptr);
    vkFreeMemory(pVulkanResources->getLogicalDevice(), vertexBufferMemory, nullptr);
    
    pVulkanResources = nullptr;
};

}

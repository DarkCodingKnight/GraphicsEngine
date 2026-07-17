#include "Mesh.hpp"

namespace Engine {

Mesh::Mesh(APImanager* pManager,
           const VkCommandPool& commandPool,
           const std::vector<Vertex>& vert,
           const std::vector<uint16_t>& ind) :
pAPImanager(pManager), vertices(vert), indices(ind)
{
    VkDeviceSize vertexBufferSize = sizeof(vertices.at(0)) * vertices.size();
    createBuffer(vertices.data(), vertexBufferSize, commandPool, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer);
    
    VkDeviceSize indexBufferSize = sizeof(indices.at(0)) * indices.size();
    createBuffer(indices.data(), indexBufferSize, commandPool, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer);
};

template<typename T>
void Mesh::createBuffer(const T* dataArray,
                        const VkDeviceSize bufferSize,
                        const VkCommandPool commandPool,
                        VkBufferUsageFlagBits bufferUsageBit,
                        VkBuffer& dstBuffer)
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
                       vertexBufferMemory);
    
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

Mesh::~Mesh() {
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), indexBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), indexBufferMemory, nullptr);
    
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBufferMemory, nullptr);
    
    pAPImanager = nullptr;
};

}

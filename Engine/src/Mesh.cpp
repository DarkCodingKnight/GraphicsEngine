#include "Mesh.hpp"

namespace Engine {

void Mesh::createVertexBuffer() {
    
    // создем буффер
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
    {
        ConsoleText::printError("Failed to create vertex buffer!", "Render program");
        std::exit(0);
    }
    else ConsoleText::printGreen("Vertex buffer was created!", "Render program");
    
    
    // выделяем память и биндим буффер
    VkMemoryRequirements memRequirnaments;
    vkGetBufferMemoryRequirements(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBuffer, &memRequirnaments);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirnaments.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirnaments.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    if (vkAllocateMemory(pAPImanager->getVulkanResources()->getLogicalDevice(),
                         &allocInfo,
                         nullptr,
                         &vertexBufferMemory) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate vertex buffer memory!", "Render program");
        std::exit(0);
    }
    else ConsoleText::printGreen("Buffer memory was allocated!", "Render program");
    
    vkBindBufferMemory(pAPImanager->getVulkanResources()->getLogicalDevice(),
                       vertexBuffer,
                       vertexBufferMemory,
                       0);
    
    
    // переносим данные на видеокарту
    void* data;
    vkMapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBufferMemory);
}

uint32_t Mesh::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(pAPImanager->getVulkanResources()->getPhysicalDevice(), &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    ConsoleText::printError("Failed to find suitable memory type!", "Render program");
    std::exit(0);
}

Mesh::~Mesh() {
    vkDestroyBuffer(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBuffer, nullptr);
    vkFreeMemory(pAPImanager->getVulkanResources()->getLogicalDevice(), vertexBufferMemory, nullptr);
    
    pAPImanager = nullptr;
};

}

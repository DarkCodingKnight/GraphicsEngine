#pragma once

#include <array>
#include <glm/glm.hpp>

#include "Pipeline.hpp"
#include "VulkanData.hpp"

namespace Engine {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }
    
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        return attributeDescriptions;
    }
};

class Mesh {
public:
    Mesh() = delete;
    Mesh(APImanager* pManager,
         const VkCommandPool& commandPool,
         const std::vector<Vertex>& vert,
         const std::vector<uint16_t>& ind);
    
    size_t getVerticesSize() { return vertices.size(); };
    size_t getIndicesSize() { return indices.size(); };
    VkBuffer* getVertexBufferPointer() { return &vertexBuffer; };
    VkBuffer* getIndexBufferPointer() { return &indexBuffer; };
    
    ~Mesh();
    
private:
    template<typename T>
    void createBuffer(const T* dataArray,
                      const VkDeviceSize bufferSize,
                      const VkCommandPool commandPool,
                      VkBufferUsageFlagBits bufferUsageBit,
                      VkBuffer& dstBuffer);
    
    void copyBuffer(VkBuffer srcBuffer,
                    VkBuffer dstBuffer,
                    VkDeviceSize size,
                    const VkCommandPool& commandPool);
    
    APImanager* pAPImanager = nullptr;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

}

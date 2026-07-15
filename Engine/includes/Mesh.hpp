#pragma once

#include <array>

#include "Pipeline.hpp"

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
    Mesh(APImanager* pManager, const std::vector<Vertex>& vert) : pAPImanager(pManager), vertices(vert) {};
    
    void createVertexBuffer();
    size_t getVerticesSize() { return vertices.size(); };
    VkBuffer& getBuffer() { return vertexBuffer; };
    
    ~Mesh();
    
private:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    APImanager* pAPImanager = nullptr;
    std::vector<Vertex> vertices;
    
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

}

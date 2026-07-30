#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <chrono>
#include <array>

#include "Pipeline.hpp"
#include "VulkanData.hpp"
#include "Texture.hpp"

namespace Engine {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }
    
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        
        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 project;
};

class Mesh {
public:
    Mesh() = delete;
    Mesh(APImanager* pManager,
         const VkCommandPool& commandPool,
         const std::vector<Vertex>& vert,
         const std::vector<uint16_t>& ind,
         const int _frames_,
         const VkDescriptorSetLayout& descriptorSetLayout);
    
    size_t getVerticesSize() { return vertices.size(); };
    size_t getIndicesSize() { return indices.size(); };
    VkBuffer* getVertexBufferPointer() { return &vertexBuffer; };
    VkBuffer* getIndexBufferPointer() { return &indexBuffer; };
    const VkDescriptorSet* getDescriptorSet(int currentFrame) { return &(descriptorSets.at(currentFrame)); };
    
    void updateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);
    
    ~Mesh();
    
private:
    template<typename T>
    void createBuffer(const T* dataArray,
                      const VkDeviceSize bufferSize,
                      const VkCommandPool commandPool,
                      VkBufferUsageFlagBits bufferUsageBit,
                      VkBuffer& dstBuffer,
                      VkDeviceMemory& dstMemory);
    
    void copyBuffer(VkBuffer srcBuffer,
                    VkBuffer dstBuffer,
                    VkDeviceSize size,
                    const VkCommandPool& commandPool);
    
    void createUniformBuffers(int max_frames_in_flight);
    
    void createDescriptorPool();
    void createDescriptorSets(const VkDescriptorSetLayout& descriptorSetLayout,
                              VkImageView& textureImageView,
                              VkSampler& textureImageSampler);
    
    APImanager* pAPImanager = nullptr;
    Texture* pTexture = nullptr;
    
    const char* texturePath = "/Users/user/Projects/GraphicsEngine/Engine/textures/lotus.jpg";
    
    //const char* texturePath = "../../../Engine/textures/lotus.jpg";
    
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    
    // buffers
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    
    // descriptors
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    
    int max_frames_in_flight = 0;
};

}

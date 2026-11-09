#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <vector>

#include "Mesh.hpp"
#include "DescriptorResources.hpp"

namespace Engine {

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 project;
};

class GraphicsObject {
public:
    GraphicsObject() = delete;
    GraphicsObject(const GraphicsObject& other) = delete;
    GraphicsObject(GraphicsObject&& other) = delete;
    GraphicsObject& operator=(const GraphicsObject& other) = delete;
    GraphicsObject& operator=(GraphicsObject&& other) = delete;
    
    GraphicsObject(VulkanResources* vulkanResources,
                   const VkCommandPool& commandPool,
                   const VkDescriptorSetLayout& descriptorSetLayout,
                   const VkDescriptorPool& descriptorPool,
                   const VkSampler& textureSampler,
                   const char* texturePath,
                   const char* modelPath,
                   const uint16_t max_frames,
                   std::string object_name);
    
    void draw();
    bool isDrawEnable();
    void scale(float x_scale, float y_scale, float z_scale);
    void rotate(float angle, bool x_axis, bool y_axis, bool z_axis);
    void translate(float x_axis, float y_axis, float z_axis);
    
    std::string getObjectName() { return objectName; }
    glm::vec3* getPositionVec() { return &position_vec; };
    glm::vec3* getScaleVec() { return &scale_vec; };
    bool* setShowWindowParameter() { return &showParametersWindow; }
    
    uint32_t getVertexBufferSize() { return pMesh->getVerticesSize(); };
    uint32_t getIndexBufferSize() { return pMesh->getIndicesSize(); };
    VkBuffer* getVertexBufferPointer() { return pMesh->getVertexBufferPointer(); };
    VkBuffer& getIndexBuffer() { return pMesh->getIndexBuffer(); };
    VkDescriptorSet* getDescriptorSets(glm::mat4 view_matrix,
                                       glm::mat4 proj_matrix,
                                       uint16_t currentFrame);
    
    ~GraphicsObject();
    
private:
    glm::mat4 getModelMatrix();
    void createUniformBuffers();
    void createDescriptorSets(const VkDescriptorPool& descriptorPool,
                              const VkSampler& textureSampler,
                              const VkDescriptorSetLayout& descriptorSetLayout);
    
    VulkanResources* pVulkanResources = nullptr;
    Mesh* pMesh = nullptr;
    Texture* pTexture = nullptr;
    
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    std::vector<VkDescriptorSet> descriptorSets;
    
    uint8_t max_frames_in_flight = 0;
    bool drawObject = false;
    
    glm::vec3 scale_vec = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 position_vec = glm::vec3(0.0f, 0.0f, 0.0f);
    float rotation_angle = 0.0f;
    glm::vec3 rotation_axis_vec = glm::vec3(0.0f, 0.0f, 0.0f);
    
    std::string objectName = "";
    bool showParametersWindow = false;
};

}

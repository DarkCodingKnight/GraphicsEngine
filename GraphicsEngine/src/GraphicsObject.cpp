#include "GraphicsObject.hpp"

namespace Engine {

GraphicsObject::GraphicsObject(VulkanResources* vulkanResources,
                               const VkCommandPool& commandPool,
                               const VkDescriptorSetLayout& descriptorSetLayout,
                               const VkDescriptorPool& descriptorPool,
                               const VkSampler& textureSampler,
                               const char* texturePath,
                               const char* modelPath,
                               const uint16_t max_frames) :
pVulkanResources(vulkanResources), max_frames_in_flight(max_frames)
{
    pMesh = new Mesh(pVulkanResources,
                     modelPath,
                     commandPool,
                     max_frames_in_flight);
    
    pTexture = new Texture(pVulkanResources, texturePath, commandPool);
    
    createUniformBuffers();
    createDescriptorSets(descriptorPool, textureSampler, descriptorSetLayout);
}

void GraphicsObject::draw() {
    drawObject = true;
}

bool GraphicsObject::isDrawEnable() {
    if (drawObject) {
        drawObject = false;
        return true;
    }
    else return false;
}

void GraphicsObject::scale(float x_axis, float y_axis, float z_axis)
{
    scale_vec += glm::vec3(x_axis, y_axis, z_axis);
}

void GraphicsObject::rotate(float angle, bool x_axis, bool y_axis, bool z_axis)
{
    rotation_angles.push_back(angle);
    rotation_axes.push_back(glm::vec3((float)x_axis, (float)y_axis, (float)z_axis));
}

void GraphicsObject::translate(float x_axis, float y_axis, float z_axis) {
    position_vec += glm::vec3(x_axis, y_axis, z_axis);
    
    
    std::cout << "----------------------- "
    << " X position: " << position_vec[0]
    << " Y position: " << position_vec[1]
    << " Z position: " << position_vec[2]
    << " -----------------------" << std::endl;
}

VkDescriptorSet* GraphicsObject::getDescriptorSets(uint16_t currentFrame)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    UniformBufferObject ubo{};
    ubo.model = getModelMatrix();
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.project = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
    ubo.project[1][1] *= -1;
    
    memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
    
    return &descriptorSets[currentFrame];
}

glm::mat4 GraphicsObject::getModelMatrix() {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    
    model_matrix = glm::scale(model_matrix, scale_vec);
    
    for(size_t i = 0; i < rotation_axes.size(); i++) {
        model_matrix = glm::rotate(model_matrix, rotation_angles.at(i), rotation_axes.at(i));
    }
    
    model_matrix = glm::translate(model_matrix, position_vec);
    
    return model_matrix;
}

void GraphicsObject::createDescriptorSets(const VkDescriptorPool& descriptorPool,
                                  const VkSampler& textureSampler,
                                  const VkDescriptorSetLayout& descriptorSetLayout)
{
    std::vector<VkDescriptorSetLayout> layouts(max_frames_in_flight, descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(max_frames_in_flight);
    allocInfo.pSetLayouts = layouts.data();
    
    descriptorSets.resize(max_frames_in_flight);
    
    if (vkAllocateDescriptorSets(pVulkanResources->getLogicalDevice(),
                                 &allocInfo,
                                 descriptorSets.data()) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate descriptor sets!", "Pipeline");
        std::exit(0);
    }
    else ConsoleText::printGreen("Descriptor sets were allocated!", "Pipeline");
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        VkDescriptorBufferInfo uniformBufferInfo{};
        uniformBufferInfo.buffer = uniformBuffers[i];
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(UniformBufferObject);
        
        VkWriteDescriptorSet uniformBufferDescriptorWrite{};
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.dstSet = descriptorSets[i];
        uniformBufferDescriptorWrite.dstBinding = 0;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.descriptorCount = 1;
        uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferInfo;
        
        
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = pTexture->getImageView();
        imageInfo.sampler = textureSampler;
        
        VkWriteDescriptorSet textureSamplerDescriptorWrite{};
        textureSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        textureSamplerDescriptorWrite.dstSet = descriptorSets[i];
        textureSamplerDescriptorWrite.dstBinding = 1;
        textureSamplerDescriptorWrite.dstArrayElement = 0;
        textureSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerDescriptorWrite.descriptorCount = 1;
        textureSamplerDescriptorWrite.pImageInfo = &imageInfo;
        
        
        std::vector<VkWriteDescriptorSet> descriptorWrites = { uniformBufferDescriptorWrite, textureSamplerDescriptorWrite };
        
        vkUpdateDescriptorSets(pVulkanResources->getLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void GraphicsObject::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    
    uniformBuffers.resize(max_frames_in_flight);
    uniformBuffersMemory.resize(max_frames_in_flight);
    uniformBuffersMapped.resize(max_frames_in_flight);
    
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        createBufferSource(pVulkanResources->getLogicalDevice(),
                           pVulkanResources->getPhysicalDevice(),
                           bufferSize,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           uniformBuffers[i],
                           uniformBuffersMemory[i]);
        
        vkMapMemory(pVulkanResources->getLogicalDevice(),
                    uniformBuffersMemory[i],
                    0,
                    bufferSize,
                    0,
                    &uniformBuffersMapped[i]);
    }
}

GraphicsObject::~GraphicsObject() {
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        vkDestroyBuffer(pVulkanResources->getLogicalDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(pVulkanResources->getLogicalDevice(), uniformBuffersMemory[i], nullptr);
    }
    
    delete pMesh;
    delete pTexture;
    
    pVulkanResources = nullptr;
    pMesh = nullptr;
    pTexture = nullptr;
};

}

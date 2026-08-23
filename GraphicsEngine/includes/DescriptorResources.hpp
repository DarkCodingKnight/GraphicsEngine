#pragma once

#include "VulkanResources.hpp"

namespace Engine {

class DescriptorPool {
public:
    virtual const VkDescriptorPool& getdescriptorPool() const = 0;
    virtual ~DescriptorPool() = default;
};


class IMGUIDescriptorPool : public DescriptorPool {
public:
    IMGUIDescriptorPool() = delete;
    IMGUIDescriptorPool(VulkanResources* _pVulkanResources_,
                        uint16_t _max_frames_in_flight_);
    
    const VkDescriptorPool& getdescriptorPool() const override { return descriptorPool; }
    
    ~IMGUIDescriptorPool() override;
    
private:
    void createDescriptorPool(VulkanResources* _pVulkanResources_,
                              uint16_t _max_frames_in_flight_);
    
    VkDescriptorPool descriptorPool;
    VulkanResources* pVulkanResources = nullptr;
};


class EngineDescriptorPool : public DescriptorPool {
public:
    EngineDescriptorPool() = delete;
    EngineDescriptorPool(VulkanResources* _pVulkanResources_,
                         uint16_t _max_frames_in_flight_);
    
    const VkDescriptorPool& getdescriptorPool() const override { return descriptorPool; }
    
    ~EngineDescriptorPool() override;
    
private:
    void createDescriptorPool(VulkanResources* _pVulkanResources_,
                              uint16_t _max_frames_in_flight_);

    VkDescriptorPool descriptorPool;
    VulkanResources* pVulkanResources = nullptr;
};


class DescriptorResources {
public:
    DescriptorResources() = delete;
    DescriptorResources(const DescriptorResources& other) = delete;
    DescriptorResources(DescriptorResources&& other) = delete;
    DescriptorResources& operator=(const DescriptorResources& other) = delete;
    DescriptorResources& operator=(DescriptorResources&& other) = delete;
    
    DescriptorResources(VulkanResources* _pVulkanResources_,
                        uint16_t _max_frames_in_flight_);
    
    VkDescriptorSetLayout* getDescriptorSetLayout() { return &descriptorSetLayout; }
    std::unique_ptr<DescriptorPool> getEngineDescriptorPool() { return std::make_unique<EngineDescriptorPool>(pVulkanResources, max_frames_in_flight); }
    std::unique_ptr<DescriptorPool> getIMGUIDescriptorPool() { return std::make_unique<IMGUIDescriptorPool>(pVulkanResources, max_frames_in_flight); }
    
    ~DescriptorResources();
    
private:
    void createDescriptorSetLayout();
    
    uint16_t max_frames_in_flight = 2;
    
    VkDescriptorSetLayout descriptorSetLayout;
    
    VulkanResources* pVulkanResources = nullptr;
};

}

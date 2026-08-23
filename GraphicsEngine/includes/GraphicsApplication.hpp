#pragma once

#include <memory>

#include "Renderer.hpp"
#include "Camera.hpp"
#include "DescriptorResources.hpp"
#include "Audio.hpp"

namespace Engine {

struct GraphicsApplicationInfo {
    const char* application_name;
    uint16_t window_width;
    uint16_t window_height;
    bool enable_validation_Layers;
    uint16_t max_frames_in_flight;
};

struct ObjectPoolInfo {
    std::string vertexPath;
    std::string fragmentPath;
    uint16_t max_frames_in_flight;
};

class GraphicsApplication {
public:
    GraphicsApplication() = delete;
    GraphicsApplication(const GraphicsApplication& other) = delete;
    GraphicsApplication(GraphicsApplication&& other) = delete;
    GraphicsApplication& operator=(const GraphicsApplication& other) = delete;
    GraphicsApplication& operator=(GraphicsApplication&& other) = delete;
    
    GraphicsApplication(GraphicsApplicationInfo* pAppInfo);
    
    std::weak_ptr<GraphicsObjectPool> createGraphicsObjectPool(ObjectPoolInfo* pPoolInfo);
    
    bool getKeyPressed(float keyNumber) { return pResourceManager->getKeyPressed(keyNumber); }
    
    bool updateGraphicsApplication();
    void cleanUpApplication();
    
    ~GraphicsApplication();
    
private:
    Window* pWindow = nullptr;
    ResourceManager* pResourceManager = nullptr;
    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
    Renderer* pRenderer = nullptr;
    DescriptorResources* pDescriptorResources = nullptr;
    Sampler* pSampler = nullptr;
    Camera* pCamera = nullptr;
    
    IMGUIwindow* pIMGUIwindow = nullptr;
    Audio* pAudio = nullptr;
    
    std::vector<std::shared_ptr<GraphicsObjectPool>> pGraphicsObjectPools = {};
    
    int windowWidth = NULL;
    int windowHeight = NULL;
    
    
};
    
}

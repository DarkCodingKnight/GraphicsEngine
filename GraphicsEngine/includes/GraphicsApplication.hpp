#pragma once

#include <memory>

#include "Renderer.hpp"
#include "Camera.hpp"
#include "DescriptorResources.hpp"
#include "AudioManager.hpp"

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
    std::weak_ptr<IMGUImanager> create_IMGUI_manager();
    std::weak_ptr<AudioManager> create_audio_manager();
    
    bool getKeyPressed(float keyNumber) { return pResourceManager->getKeyPressed(keyNumber); }
    bool getMouseKyPressed(float mouseKyeNumber) { return pResourceManager->getMouseKeyPressed(mouseKyeNumber); }
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
    
    std::shared_ptr<IMGUImanager> pIMGUImanager = nullptr;
    std::shared_ptr<AudioManager> pAudioManager = nullptr;
    
    std::vector<std::shared_ptr<GraphicsObjectPool>> pGraphicsObjectPools = {};
    
    int windowWidth = NULL;
    int windowHeight = NULL;
};
    
}

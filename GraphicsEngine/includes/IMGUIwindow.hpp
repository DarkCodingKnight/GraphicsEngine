#pragma once

#define IMGUI_VULKAN_DEBUG_REPORT
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "SwapChain.hpp"
#include "DescriptorResources.hpp"
#include "Camera.hpp"
#include "Audio.hpp"
#include "GraphicsObject.hpp"

namespace Engine {

class IMGUIwindow {
public:
    IMGUIwindow(VulkanResources* _pVulkanResources_,
                SwapChain* _pSwapChain_,
                DescriptorResources* _pDescriptorResources_,
                Audio* _pAudio_,
                GLFWwindow* _pGLFWwindow_);
    
    void beginFrame(Camera* _pCamera_,
                    float window_width,
                    float window_height);
    void render(Camera* _pCamera_,
                VkCommandBuffer& commandBuffer);
    void setGraphicsObject(std::shared_ptr<GraphicsObject> pGraphicsObject);
    
    ~IMGUIwindow();
    
private:
    void initIMGUIwindow(DescriptorResources* _pDescriptorResources_,
                         const VkRenderPass& renderPass,
                         GLFWwindow* _pGLFWwindow_);
    
    std::unique_ptr<DescriptorPool> pIMGUIDescriptorPool;
    
    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
    Audio* pAudio;
    std::vector<std::shared_ptr<GraphicsObject>> pGraphicsObjects;
    
    float sinus = 1;
};

}

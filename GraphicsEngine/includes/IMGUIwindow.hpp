#pragma once

#define IMGUI_VULKAN_DEBUG_REPORT
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "SwapChain.hpp"
#include "DescriptorResources.hpp"

namespace Engine {

class IMGUIwindow {
public:
    IMGUIwindow(VulkanResources* _pVulkanResources_,
                SwapChain* _pSwapChain_,
                DescriptorResources* _pDescriptorResources_,
                GLFWwindow* _pGLFWwindow_);
    
    void beginFrame();
    void render(VkCommandBuffer& commandBuffer);
    
    ~IMGUIwindow();
    
private:
    void initIMGUIwindow(DescriptorResources* _pDescriptorResources_,
                         const VkRenderPass& renderPass,
                         GLFWwindow* _pGLFWwindow_);
    
    std::unique_ptr<DescriptorPool> pIMGUIDescriptorPool;
    
    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
};

}

#pragma once

#define IMGUI_VULKAN_DEBUG_REPORT
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "implot.h"

#include "SwapChain.hpp"
#include "DescriptorResources.hpp"
#include "Camera.hpp"
#include "Audio.hpp"
#include "GraphicsObject.hpp"

namespace Engine {

class IMPLOTwindow {
public:
    IMPLOTwindow(const char* _graphic_name_ = " ",
                 const char* _line_name_ = " ",
                 const char* _x_axis_name_ = " ",
                 const char* _y_axis_name_ = " ") :
    graphic_name(_graphic_name_),
    line_name(_line_name_),
    x_axis_name(_x_axis_name_),
    y_axis_name(_y_axis_name_) {}
    
    ~IMPLOTwindow() = default;
    
    void draw();
    void set_value(float value) { Y_axis.emplace_back(value); }
    
private:
    const char* graphic_name;
    const char* line_name;
    const char* x_axis_name;
    const char* y_axis_name;
    
    std::vector<float> X_axis;
    std::vector<float> Y_axis;
    
    uint32_t count = 0;
};

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
    
    std::weak_ptr<IMPLOTwindow> create_ImPlot_window(const char* _graphic_name_,
                                                     const char* _line_name_,
                                                     const char* _x_axis_name_,
                                                     const char* _y_axis_name_);
    
    ~IMGUIwindow();
    
private:
    void initIMGUIwindow(DescriptorResources* _pDescriptorResources_,
                         const VkRenderPass& renderPass,
                         GLFWwindow* _pGLFWwindow_);
    void drawSignalWindow();
    
    std::unique_ptr<DescriptorPool> pIMGUIDescriptorPool;
    
    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
    Audio* pAudio;
    std::vector<std::shared_ptr<GraphicsObject>> pGraphicsObjects;
    
    std::vector<std::shared_ptr<IMPLOTwindow>> pIMPLOTwindows;
    
    float sinus = 1.0f;
};

}

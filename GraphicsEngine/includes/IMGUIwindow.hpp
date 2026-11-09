#pragma once

#include "IMPLOTwindow.hpp"

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
    
    std::weak_ptr<IMPLOTwindow> create_ImPlot_window(const char* _graphic_name_,
                                                     const char* _x_axis_name_,
                                                     const char* _y_axis_name_,
                                                     float _step_);
    
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
    
    std::vector<std::shared_ptr<IMPLOTwindow>> pIMPLOTwindows;
    
    float sinus = 1.0f;
};

}

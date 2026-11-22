#pragma once

#include "IMGUIwindow.hpp"
#include "IMPLOTwindow.hpp"

namespace Engine {

class IMGUImanager {
public:
    IMGUImanager() = delete;
    IMGUImanager(const IMGUImanager& other) = delete;
    IMGUImanager(IMGUImanager&& other) = delete;
    IMGUImanager& operator=(const IMGUImanager& other) = delete;
    IMGUImanager& operator=(IMGUImanager&& other) = delete;
    IMGUImanager(VulkanResources* const _pVulkanResources_,
                 SwapChain* const _pSwapChain_,
                 DescriptorResources* const _pDescriptorResources_,
                 GLFWwindow* const _pGLFWwindow_);

    void init(DescriptorResources* const _pDescriptorResources_);
    void render(Camera* _pCamera_, VkCommandBuffer& commandBuffer);
    void beginFrame(Camera* _pCamera_,
                    float window_width,
                    float window_height);
    std::weak_ptr<IMGUIwindow> create_IMGUIwindow(std::string _name_);
    std::weak_ptr<IMPLOTwindow> create_IMPLOTwindow(const char* _graphic_name_,
                                                    const char* _x_axis_name_,
                                                    const char* _y_axis_name_,
                                                    float _step_);

    ~IMGUImanager();

private:
    void cleanup();

    VulkanResources* pVulkanResources = nullptr;
    SwapChain* pSwapChain = nullptr;
    GLFWwindow* pGLFWwindow = nullptr;

    std::vector<std::shared_ptr<IMGUIwindow>> pIMGUIwindows;
    std::vector<std::shared_ptr<IMPLOTwindow>> pIMPLOTwindows;
};

}
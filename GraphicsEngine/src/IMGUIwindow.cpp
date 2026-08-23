#include "IMGUIwindow.hpp"

namespace Engine {

IMGUIwindow::IMGUIwindow(VulkanResources* _pVulkanResources_,
                         SwapChain* _pSwapChain_,
                         DescriptorResources* _pDescriptorResources_,
                         Audio* _pAudio_,
                         GLFWwindow* _pGLFWwindow_) :
pVulkanResources(_pVulkanResources_), pSwapChain(_pSwapChain_), pAudio(_pAudio_)
{
    initIMGUIwindow(_pDescriptorResources_, pSwapChain->getSwapChainRenderPass(), _pGLFWwindow_);
}

void IMGUIwindow::initIMGUIwindow(DescriptorResources* _pDescriptorResources_,
                                  const VkRenderPass& renderPass,
                                  GLFWwindow* _pGLFWwindow_)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontDefault();
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForVulkan(_pGLFWwindow_, true);
    
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = pVulkanResources->getInstance();
    init_info.PhysicalDevice = pVulkanResources->getPhysicalDevice();
    init_info.Device = pVulkanResources->getLogicalDevice();
    init_info.QueueFamily = pVulkanResources->getQueueFamilyIndices().graphicsFamily.value();
    init_info.Queue = pVulkanResources->getGraphicsQueue();
    
    pIMGUIDescriptorPool = _pDescriptorResources_->getIMGUIDescriptorPool();
    
    init_info.DescriptorPool = pIMGUIDescriptorPool->getdescriptorPool();
    init_info.MinImageCount = static_cast<uint32_t>(pSwapChain->getSwapChainImageViews().size());
    init_info.ImageCount = static_cast<uint32_t>(pSwapChain->getSwapChainImageViews().size());
    //init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = nullptr;
    
    init_info.PipelineInfoMain.RenderPass = renderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    
    ImGui_ImplVulkan_Init(&init_info);
    
    ConsoleText::printGreen("ImGui was initialized!", "ImGui");
}

void IMGUIwindow::beginFrame(Camera* _pCamera_,
                             float window_width,
                             float window_height)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    
    ImGui::NewFrame();
    
    ImGui::Begin("Vulkan engine VERSION_1.2");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    
    if (_pCamera_ != nullptr) {
        static float camera_speed = 1.0f;
        static float mouse_sensitivity = 0.5f;
        static float z_near = 0.1f;
        static float z_far = 1000.0f;
        static float field_of_view = 45.0f;
        
        ImGui::Text("Camera info: ");
        ImGui::SliderFloat("Camera speed", &camera_speed, 0.1f, 1000.0f);
        ImGui::SliderFloat("Mouse sensitivity", &mouse_sensitivity, 0.1f, 10.0f);
        ImGui::SliderFloat("Z near", &z_near, 0.1f, 10.0f);
        ImGui::SliderFloat("Z far", &z_far, 0.1f, 10000.0f);
        ImGui::SliderFloat("Field of view", &field_of_view, 30.0f, 90.0f);
        
        _pCamera_->setCameraSpeed(camera_speed);
        _pCamera_->setMouseSensitivity(mouse_sensitivity);
        
        _pCamera_->setProjectionMatrix(window_width, window_height, z_near, z_far, field_of_view);
    }
    
    if (pAudio != nullptr) {
        static float volume = 0.5f;
        
        ImGui::Text("Audio info: ");
        ImGui::SliderFloat("Audio volume: ", &volume, 0.0f, 1.0f);
        
        pAudio->setVolume(volume);
    }
    
    ImGui::End();
}

void IMGUIwindow::render(VkCommandBuffer& commandBuffer) {
    ImGui::Render();
    
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

IMGUIwindow::~IMGUIwindow() {
    vkDeviceWaitIdle(pVulkanResources->getLogicalDevice());
    
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    
    ImGui::DestroyContext();
    
    ConsoleText::printGreen("ImGui was destroyed!", "ImGui");
    
    pVulkanResources = nullptr;
    pSwapChain = nullptr;
    pAudio = nullptr;
}

}

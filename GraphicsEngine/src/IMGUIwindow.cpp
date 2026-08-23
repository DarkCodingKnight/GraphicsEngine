#include "IMGUIwindow.hpp"

namespace Engine {

IMGUIwindow::IMGUIwindow(VulkanResources* _pVulkanResources_,
                         SwapChain* _pSwapChain_,
                         DescriptorResources* _pDescriptorResources_,
                         GLFWwindow* _pGLFWwindow_) :
pVulkanResources(_pVulkanResources_), pSwapChain(_pSwapChain_)
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

void IMGUIwindow::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    
    ImGui::NewFrame();
    
    ImGui::Begin("My window");
    ImGui::Text("Hello, world!");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    
    if (ImGui::Button("Click Me")) {
        ConsoleText::printGreen("Button clicked!", "IMGUI");
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
}

}

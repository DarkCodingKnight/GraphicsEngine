#include "IMGUImanager.hpp"

namespace Engine {

IMGUImanager::IMGUImanager(VulkanResources* const _pVulkanResources_,
                           SwapChain* const _pSwapChain_,
                           DescriptorResources const _pDescriptorResources_,
                           GLFWwindow* const _pGLFWwindow_) :
    pVulkanResources(_pVulkanResources_),
    pSwapChain(_pSwapChain_),
    pGLFWwindow(_pGLFWwindow_)
{
    init(_pDescriptorResources_);
}

void IMGUImanager::init(DescriptorResources* const _pDescriptorResources_) {

    // ----- ImGui initializing -----

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontDefault();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(pGLFWwindow, true);

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

    // ImPlot initializing
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ConsoleText::printGreen("ImGui was initialized!", "IMGUImanager");
}

void IMGUIwindow::beginFrame(Camera* _pCamera_,
                             float window_width,
                             float window_height)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    for (auto pIMPLOTwindow : pIMPLOTwindows) { pIMPLOTwindow->draw(); }
    for (auto pIMGUIwindow : pIMGUIwindows) { pIMGUIwindow->draw(); }
}

void IMGUIwindow::render(Camera* _pCamera_,
                         VkCommandBuffer& commandBuffer) {
    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

std::weak_ptr<IMGUIwindow> IMGUImanager::create_IMGUIwindow(std::string _name_) {
    std::shared_ptr<IMGUIwindow> pIMGUIwindow = std::shared_ptr<IMGUIwindow>(_name_);
    pIMGUIwindow.emplace_back(pIMGUIwindow);
    return pIMGUIwindow;
}

std::weak_ptr<IMPLOTwindow> IMGUImanager::create_IMPLOTwindow(const char* _graphic_name_,
                                                              const char* _x_axis_name_,
                                                              const char* _y_axis_name_,
                                                              float _step_)
{
    std::shared_ptr<IMPLOTwindow> pIMPLOTwindow = std::make_shared<IMPLOTwindow>(_graphic_name_,
                                                                                 _x_axis_name_,
                                                                                 _y_axis_name_,
                                                                                 _step_);
    pIMPLOTwindows.emplace_back(pIMPLOTwindow);
    return pIMPLOTwindow;
}

void IMGUImanager::cleanup() {
    for (auto pIMGUIwindow : pIMGUIwindows) { pIMGUIwindow.reset(); }
    for (auto pIMPLOTwindow : pIMPLOTwindow) { pIMPLOTwindow.reset(); }
}

IMGUImanager::~IMGUImanager() {
    vkDeviceWaitIdle(pVulkanResources->getLogicalDevice());

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    ConsoleText::printGreen("ImGui was destroyed!", "ImGui");

    pVulkanResources = nullptr;
    pSwapChain = nullptr;
    pGLFWwindow = nullptr;
}

}



/*
 *     ImGui::Begin("Vulkan engine VERSION_1.2");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
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
    ImGui::Begin(objectName);
    ImGui::SliderFloat("Position X: ", &pGraphicsObject->getPositionVec()->x, -50.0f, 50.0f);
    ImGui::SliderFloat("Position Y: ", &pGraphicsObject->getPositionVec()->y, -50.0f, 50.0f);
    ImGui::SliderFloat("Position Z: ", &pGraphicsObject->getPositionVec()->z, -50.0f, 50.0f);
    ImGui::End();
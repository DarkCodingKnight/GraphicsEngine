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

std::weak_ptr<IMGUIwindow> IMGUImanager::create_IMGUIwindow() {

}

std::weak_ptr<IMPLOTwindow> IMGUImanager::create_IMPLOTwindow(const char* _graphic_name_,
                                                              const char* _x_axis_name_,
                                                              const char* _y_axis_name_,
                                                              float _step_)
{
    std::shared_ptr<IMPLOTwindow> pIMPLOTwindow = std::make_shared<IMPLOTwindow>(_graphic_name_, _x_axis_name_, _y_axis_name_, _step_);

    pIMPLOTwindows.emplace_back(pIMPLOTwindow);
    return pIMPLOTwindow;
}

IMGUImanager::~IMGUImanager() {
    pVulkanResources = nullptr;
    pSwapChain = nullptr;
    pGLFWwindow = nullptr;
}

}
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
    // ImGui initializing
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
    
    // ImPlot initializing
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ConsoleText::printGreen("ImGui was initialized!", "ImGui");
}

void IMGUIwindow::setGraphicsObject(std::shared_ptr<GraphicsObject> pGraphicsObject) {
    pGraphicsObjects.emplace_back(pGraphicsObject);
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
    
    //drawSignalWindow();
    
    for (auto pIMPLOTwindow : pIMPLOTwindows) {
        pIMPLOTwindow->draw();
    }
    
    static bool isCameraEnable = false;
    ImGui::Checkbox("Show Camera parameters", &isCameraEnable);
    
    static bool isAudioEnable = false;
    ImGui::Checkbox("Show Audio parameters", &isAudioEnable);
    
    for (auto pGraphicsObject : pGraphicsObjects) {
        std::string object_name = "Show " + pGraphicsObject->getObjectName() + " parameters";
        ImGui::Checkbox(object_name.c_str(), pGraphicsObject->setShowWindowParameter());
    }
    
    if (_pCamera_ != nullptr && isCameraEnable) {
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
    
    if (pAudio != nullptr && isAudioEnable) {
        static float volume = 0.5f;
        
        ImGui::Text("Audio info: ");
        ImGui::SliderFloat("Audio volume: ", &volume, 0.0f, 1.0f);
        
        pAudio->setVolume(volume);
    }
    
    for (auto pGraphicsObject : pGraphicsObjects) {
        if (*pGraphicsObject->setShowWindowParameter()) {
            std::string name = pGraphicsObject->getObjectName();
            const char* objectName = name.c_str();
            ImGui::Begin(objectName);
            
            ImGui::SliderFloat("Position X: ", &pGraphicsObject->getPositionVec()->x, -50.0f, 50.0f);
            ImGui::SliderFloat("Position Y: ", &pGraphicsObject->getPositionVec()->y, -50.0f, 50.0f);
            ImGui::SliderFloat("Position Z: ", &pGraphicsObject->getPositionVec()->z, -50.0f, 50.0f);
            
            ImGui::End();
        }
    }
    
    ImGui::End();
}

void IMGUIwindow::render(Camera* _pCamera_,
                         VkCommandBuffer& commandBuffer) {
    if (_pCamera_ != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        
        if (io.WantCaptureMouse) _pCamera_->setDirectionUpdate(false);
        else _pCamera_->setDirectionUpdate(true);
    }
    
    ImGui::Render();
    
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void IMPLOTwindow::draw() {
    ImGui::Begin(graphic_name);
    
    if (ImPlot::BeginPlot(graphic_name, ImVec2(-1, 300))) {
        ImPlot::SetupAxes(x_axis_name, y_axis_name);
        //ImPlot::SetupAxes("t", "omega",
          //                    ImPlotAxisFlags_AutoFit,   // X — авто
            //                  ImPlotAxisFlags_AutoFit);
        ImPlot::PlotLine(line_name, X_axis.data(), Y_axis.data(), (int)(X_axis.size()));
        ImPlot::EndPlot();
    }
    ImGui::End();
    
    X_axis.emplace_back(count);
    count++;
}

std::weak_ptr<IMPLOTwindow> IMGUIwindow::create_ImPlot_window(const char* _graphic_name_,
                                                              const char* _line_name_,
                                                              const char* _x_axis_name_,
                                                              const char* _y_axis_name_)
{
    std::shared_ptr<IMPLOTwindow> pIMPLOTwindow = std::make_shared<IMPLOTwindow>(_graphic_name_, _line_name_, _x_axis_name_, _y_axis_name_);
    
    pIMPLOTwindows.emplace_back(pIMPLOTwindow);
    return pIMPLOTwindow;
}

void IMGUIwindow::drawSignalWindow() {
    static float history[256] = {};
    static int head = 0;
    
    history[head] = sinus;
    head = (head + 1) % 256;
    
    sinus = sin(sinus);
    if (sinus <= 0.1f) sinus = 1.0f;
    
    ImGui::Begin("Signal");
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 size(300, 100);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    
    dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), IM_COL32(30, 30, 30, 25));
    
    for (int i = 0; i < 255; ++i) {
        float y0 = p.y + size.y * (1.0f - history[(head + i) % 256]);
        float y1 = p.y + size.y * (1.0f - history[(head + i + 1) % 256]);
        
        dl->AddLine(ImVec2(p.x + i, y0), ImVec2(p.x + i + 1, y1), IM_COL32(0, 255, 0, 255));
    }
    
    //ImGui::Dummy(size);
    ImGui::End();
}

IMGUIwindow::~IMGUIwindow() {
    vkDeviceWaitIdle(pVulkanResources->getLogicalDevice());
    
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    
    ConsoleText::printGreen("ImGui was destroyed!", "ImGui");
    
    pVulkanResources = nullptr;
    pSwapChain = nullptr;
    pAudio = nullptr;
}

}

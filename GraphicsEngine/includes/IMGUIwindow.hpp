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

#include <atomic>

namespace Engine {

class IMPLOTline {
public:
    IMPLOTline(const char* _name_) : name(_name_) {}

    void draw(int count, float step, int offset) {
        if (values.empty() || count <= 0) return;
        if (offset < 0 || offset + count > (int)values.size()) return;
        ImPlot::PlotLine(name, values.data() + offset, count, step, offset * step);
    }

    void add_value(float value) {
        if (!std::isfinite(value) || !state.load(std::memory_order_relaxed)) return;
        values.emplace_back(value);
    }

    void set_state(bool _state_) {
        state.store(_state_, std::memory_order_relaxed);
    }

    size_t size() const { return values.size(); }

    ~IMPLOTline() = default;

private:
    const char* name;
    std::vector<float> values;
    std::atomic<bool> state{true};
};


class IMPLOTwindow {
public:
    IMPLOTwindow(const char* _graphic_name_ = " ",
                 const char* _x_axis_name_ = " ",
                 const char* _y_axis_name_ = " ",
                 float _step_ = 0.0f)
        : graphic_name(_graphic_name_)
        , x_axis_name(_x_axis_name_)
        , y_axis_name(_y_axis_name_)
        , step(_step_) {}

    ~IMPLOTwindow() = default;

    void draw() {
        ImGui::Begin(graphic_name);

        int total = pIMPLOTlines.empty() ? 0 : (int)pIMPLOTlines[0]->size();

        if (total > 1) {
            int offset = std::max(0, total - WINDOW_POINTS);
            int count  = total - offset;

            if (ImPlot::BeginPlot(graphic_name, ImVec2(-1, 300))) {
                ImPlot::SetupAxes(x_axis_name, y_axis_name,
                                  ImPlotAxisFlags_None,
                                  ImPlotAxisFlags_None);
                                  //ImPlotAxisFlags_AutoFit);

                ImPlotCond cond = paused ? ImPlotCond_Once : ImPlotCond_Always;

                ImPlot::SetupAxisLimits(ImAxis_X1,
                                        offset * step,
                                        (total - 1) * step,
                                        cond);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -2500, 2500, cond);

                for (auto& p : pIMPLOTlines) {
                    p->draw(count, step, offset);
                }

                draw_stop_button();

                ImPlot::EndPlot();
            }
        }
        ImGui::End();
    }

    std::weak_ptr<IMPLOTline> create_IMPLOTline(const char* _name_) {
        auto p = std::make_shared<IMPLOTline>(_name_);
        pIMPLOTlines.emplace_back(p);
        return p;
    }

private:
    void draw_stop_button() {
        ImVec2 plot_pos  = ImPlot::GetPlotPos();
        ImVec2 btn_size(100.0f, 30.0f);
        ImVec2 p1(plot_pos.x + 100.0f, plot_pos.y + 10.0f);
        ImVec2 p2(p1.x + btn_size.x, p1.y + btn_size.y);

        ImVec2 win_pos = ImGui::GetWindowPos();
        ImGui::SetCursorPos(ImVec2(p1.x - win_pos.x, p1.y - win_pos.y));

        if (ImGui::InvisibleButton("##btn_pause", btn_size)) {
            paused = !paused;
            for (auto& p : pIMPLOTlines) p->set_state(!paused);
        }

        ImDrawList* dl = ImPlot::GetPlotDrawList();
        ImU32 bg = ImGui::IsItemHovered()
            ? ImGui::GetColorU32(ImGuiCol_ButtonHovered)
            : ImGui::GetColorU32(ImGuiCol_Button);
        dl->AddRectFilled(p1, p2, bg, 4.0f);
        dl->AddText(ImVec2(p1.x + 10.0f, p1.y + 7.0f),
                    ImGui::GetColorU32(ImGuiCol_Text),
                    paused ? "Resume" : "Pause");
    }

    const char* graphic_name;
    const char* x_axis_name;
    const char* y_axis_name;

    int WINDOW_POINTS = 200;
    float step = 0.0f;
    bool paused = false;

    std::vector<std::shared_ptr<IMPLOTline>> pIMPLOTlines;
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
                                                     const char* _x_axis_name_,
                                                     const char* _y_axis_name_,
                                                     float _step_);
    
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

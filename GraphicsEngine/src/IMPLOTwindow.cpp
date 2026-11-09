#include "IMPLOTwindow.hpp"

namespace Engine {

void IMPLOTline::draw(int count, float step, int offset) {
    if (values.empty() || count <= 0) return;
    if (offset < 0 || offset + count > (int)values.size()) return;
    ImPlot::PlotLine(name, values.data() + offset, count, step, offset * step);
}

void IMPLOTline::add_value(float value) {
    if (!std::isfinite(value) || !state.load(std::memory_order_relaxed)) return;
    values.emplace_back(value);
}

void IMPLOTline::set_state(bool _state_) {
    state.store(_state_, std::memory_order_relaxed);
}

void IMPLOTwindow::draw() {
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

std::weak_ptr<IMPLOTline> IMPLOTwindow::create_IMPLOTline(const char* _name_) {
    auto p = std::make_shared<IMPLOTline>(_name_);
    pIMPLOTlines.emplace_back(p);
    return p;
}

void IMPLOTwindow::draw_stop_button() {
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

}

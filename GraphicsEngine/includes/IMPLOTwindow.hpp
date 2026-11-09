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

    void draw(int count, float step, int offset);
    void add_value(float value);
    void set_state(bool _state_);

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

    void draw();
    std::weak_ptr<IMPLOTline> create_IMPLOTline(const char* _name_);

private:
    void draw_stop_button();

    const char* graphic_name;
    const char* x_axis_name;
    const char* y_axis_name;

    int WINDOW_POINTS = 200;
    float step = 0.0f;
    bool paused = false;

    std::vector<std::shared_ptr<IMPLOTline>> pIMPLOTlines;
};

}

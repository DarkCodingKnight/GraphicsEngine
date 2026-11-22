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

namespace Engine {

class SliderFloat {
public:
    SliderFloat(std::string _name_,
                float _lower_border_,
                float _upper_border_,
                float* _data_);
    void draw();
    ~SliderFloat() = default;

private:
    std::string name = "";
    float lower_border = 0.0f;
    float upper_border = 0.0f;
    float* data = nullptr;
};

class DataFloat {
public:
    DataFloat(std::string _name_);
    void draw();
    ~DataFloat() = default;

private:
    std::string name = "";
};

class IMGUIwindow {
public:
    IMGUIwindow(std::string _name_);
    
    void draw();
    std::weak_ptr<SliderFloat> create_SliderFloat(std::string _name_,
                                                  float _lower_border_,
                                                  float _upper_border_,
                                                  float* _data_);
    std::weak_ptr<DataFloat> create_DataFloat(std::string _name_);

    ~IMGUIwindow() = default;
    
private:
    std::string name = "";

    std::vector<std::shared_ptr<SliderFloat>> pSliderFloats;
    std::vector<std::shared_ptr<DataFloat>> pDataFloats;
};

}

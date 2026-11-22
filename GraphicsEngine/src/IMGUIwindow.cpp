#include "IMGUIwindow.hpp"

namespace Engine {

SliderFloat::SliderFloat(std::string _name_,
                         float _lower_border_,
                         float _upper_border_,
                         float* _data_) :
    name(_name_),
    lower_border(_lower_border_),
    upper_border(_upper_border_),
    data(_data_) {}

void SliderFloat::draw() {
    const char* slider_name = name.c_str();

    ImGui::SliderFloat(slider_name, data, lower_border, upper_border);
}

DataFloat::DataFloat(std::string _name_) : name(_name_){}

void DataFloat::draw() {
    const char* data_name = name.c_str();

    ImGui::Text(data_name);
}

IMGUIwindow::IMGUIwindow(std::string _name_) : name(_name_) {}

std::weak_ptr<SliderFloat> IMGUIwindow::create_SliderFloat(std::string _name_,
                                                           float _lower_border_,
                                                           float _upper_border_,
                                                           float* _data_) {
    std::shared_ptr<SliderFloat> pSliderFloat = std::make_shared<SliderFloat>(_name_,
                                                                          _lower_border_,
                                                                          _upper_border_,
                                                                          _data_);
    pSliderFloats.emplace_back(pSliderFloat);
    return pSliderFloat;
}

std::weak_ptr<DataFloat> IMGUIwindow::create_DataFloat(std::string _name_) {
    std::shared_ptr<DataFloat> pDataFloat = std::make_shared<DataFloat>(_name_);
    pDataFloats.emplace_back(pDataFloat);
    return pDataFloat;
}

void IMGUIwindow::draw() {
    const char* window_name = name.c_str();

    ImGui::Begin(window_name);

    for (auto pSliderFloat : pSliderFloats) {
        pSliderFloat->draw();
    }

    for (auto pDataFloat : pDataFloats) {
        pDataFloat->draw();
    }

    ImGui::End();
}

}
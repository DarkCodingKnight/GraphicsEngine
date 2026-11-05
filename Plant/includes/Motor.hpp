#pragma once

#include "DRV8301.hpp"
#include "ADC.hpp"

#include "Kalman.hpp"

#include "IMGUIwindow.hpp"

namespace STM32 {

class Motor {
public:
    Motor(const Motor& other) = delete;
    Motor(Motor&& other) = delete;
    Motor& operator=(const Motor& other) = delete;
    Motor& operator=(Motor&& other) = delete;
    
    Motor() = default;
    
    void start(Engine::IMGUIwindow* const _pIMGUIwindow_);
    void update();
    
    ~Motor();
    
private:
    const char* graphic_name = "Motor";
    const char* line_name = "A";
    const char* x_name = "X";
    const char* y_name = "Y";
    
    float ia = 0.0f;
    float ib = 0.0f;
    
    Engine::IMGUIwindow* pIMGUIwindow = nullptr;
    std::weak_ptr<Engine::IMPLOTwindow> pIMPLOTwindow;
    
    DRV8301* pDRV8301 = nullptr;
    ADC* pADC = nullptr;
};

}

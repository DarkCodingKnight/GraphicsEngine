#pragma once

#include "DRV8301.hpp"
#include "PI.hpp"
#include "PWM.hpp"
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
    float id = 0.0f;
    float iq = 0.0f;
    float id_ref = 0.0f;
    float iq_ref = 0.0f;
    float i_alpha = 0.0f;
    float i_beta = 0.0f;
    
    float vd = 0.0f;
    float vq = 0.0f;
    float v_alpha = 0.0f;
    float v_beta = 0.0f;
    
    float speed = 0.0f;
    float angle = 0.0f;
    
    Engine::IMGUIwindow* pIMGUIwindow = nullptr;
    std::weak_ptr<Engine::IMPLOTwindow> pIMPLOTwindow;
    
    DRV8301* pDRV8301 = nullptr;
    ADC* pADC = nullptr;
    PWM* pPWM = nullptr;
    
    PI_Current_Values PI_values;
    PI_controller* pPI = nullptr;
    
    KalmanValues Kalman_values;
    Kalman* pKalman = nullptr;
};

}

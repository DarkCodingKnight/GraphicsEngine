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
    const char* x_name = "X";
    const char* y_name = "Y";
    
    float MAX_MODULATION_INDEX = 0.95f;
    
    float ia = 0.0f;
    float ib = 0.0f;
    float id = 0.0f;
    float iq = 0.0f;
    float id_ref = 0.0f;
    float iq_ref = 2.0f;
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
    
    // Токи
    const char* ia_line_name = "ia";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_ia;
    const char* ib_line_name = "ib";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_ib;
    const char* i_alpha_line_name = "i_alpha";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_i_alpha;
    const char* i_beta_line_name = "i_beta";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_i_beta;
    const char* i_d_line_name = "i_d";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_i_d;
    const char* i_q_line_name = "i_q";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_i_q;
    
    const char* speed_line_name = "speed";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_speed;
    const char* angle_line_name = "angle";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_angle;
    
    // ШИМ
    const char* PhA_line_name = "PhA";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_PhA;
    const char* PhB_line_name = "PhB";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_PhB;
    const char* PhC_line_name = "PhC";
    std::weak_ptr<Engine::IMPLOTline> pIMPLOTline_PhC;
    
    DRV8301* pDRV8301 = nullptr;
    ADC* pADC = nullptr;
    PWM* pPWM = nullptr;
    
    PI_Current_Values PI_values;
    PI_controller* pPI = nullptr;
    
    KalmanValues Kalman_values;
    Kalman* pKalman = nullptr;
};

}

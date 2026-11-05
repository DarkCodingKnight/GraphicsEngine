#include "Motor.hpp"

namespace STM32 {

void Motor::start(Engine::IMGUIwindow* const _pIMGUIwindow_) {
    pIMGUIwindow = _pIMGUIwindow_;
    
    pIMPLOTwindow = pIMGUIwindow->create_ImPlot_window(graphic_name, line_name, x_name, y_name);
    
    pDRV8301 = new DRV8301(DRV_PARAM::GAIN_40V);
    pADC = new ADC(pDRV8301->get_gain());
    pPWM = new PWM(ENGINE_FREQUENCY::TEN_KHZ);
    
    PI_values = {};
    PI_values.DKP = 3.7f / 2.0f;
    PI_values.DKI = 3140.0f / 2.0f;
    PI_values.QKP = 3.7f / 2.0f;
    PI_values.QKI = 3140.0f / 2.0f;
    PI_values.PHASE_RESISTANCE = 0.105f;
    PI_values.LD = 0.00012f;
    PI_values.LQ = 0.00012f;
    PI_values.MAG_FLUX = 0.01f;
    PI_values.TS = pPWM->get_TS();
    
    pPI = new PI_controller(&PI_values, pADC);
    
    Kalman_values = {};
    Kalman_values.PHASE_RESISTANCE = 0.105f;
    Kalman_values.MAG_FLUX = 0.01f;
    Kalman_values.LQ = 0.00012f;
    Kalman_values.TS = pPWM->get_TS();
    
    pKalman = new Kalman(&Kalman_values);
}

void Motor::update() {
    pADC->get_values(ia, ib);
    
    Math::CLARKE_transformation(i_alpha, i_beta, ia, ib, false);
    Math::PARK_transformation(id, iq, i_alpha, i_beta, angle, false);
    
    pPI->calculate(id, iq, id_ref, iq_ref, vd, vq);
    
    Math::PARK_transformation(v_alpha, v_beta, vd, vq, angle, true);
    
    
    
    pIMPLOTwindow.lock()->set_value(pADC->get_adc_value(ADC_RANKS::ADC_RANK_1));
}

Motor::~Motor() {
    pIMGUIwindow = nullptr;
    
    delete pDRV8301;
    pDRV8301 = nullptr;
    
    delete pADC;
    pADC = nullptr;
    
    delete pPI;
    pPI = nullptr;
    
    delete pKalman;
    pKalman = nullptr;
}

}

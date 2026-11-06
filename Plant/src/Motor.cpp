#include "Motor.hpp"

namespace STM32 {

void Motor::start(Engine::IMGUIwindow* const _pIMGUIwindow_) {
    pIMGUIwindow = _pIMGUIwindow_;
    
    pDRV8301 = new DRV8301(DRV_PARAM::GAIN_40V);
    pADC = new ADC(pDRV8301->get_gain());
    pPWM = new PWM(pADC, MAX_MODULATION_INDEX, ENGINE_FREQUENCY::TEN_KHZ);
    
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
    
    pPI = new PI_controller(&PI_values, pADC, MAX_MODULATION_INDEX);
    
    Kalman_values = {};
    Kalman_values.PHASE_RESISTANCE = 0.105f;
    Kalman_values.MAG_FLUX = 0.01f;
    Kalman_values.LQ = 0.00012f;
    Kalman_values.TS = pPWM->get_TS();
    
    pKalman = new Kalman(&Kalman_values);
    
    pPWM->calculate_space_vector(v_alpha, v_beta);
    
    pIMPLOTwindow = pIMGUIwindow->create_ImPlot_window(graphic_name, x_name, y_name, pPWM->get_TS());
    pIMPLOTline_ia = pIMPLOTwindow.lock()->create_IMPLOTline(ia_line_name);
    pIMPLOTline_ib = pIMPLOTwindow.lock()->create_IMPLOTline(ib_line_name);
    pIMPLOTline_i_alpha = pIMPLOTwindow.lock()->create_IMPLOTline(i_alpha_line_name);
    pIMPLOTline_i_beta = pIMPLOTwindow.lock()->create_IMPLOTline(i_beta_line_name);
    pIMPLOTline_speed = pIMPLOTwindow.lock()->create_IMPLOTline(speed_line_name);
    pIMPLOTline_angle = pIMPLOTwindow.lock()->create_IMPLOTline(angle_line_name);
    pIMPLOTline_PhA = pIMPLOTwindow.lock()->create_IMPLOTline(PhA_line_name);
    pIMPLOTline_PhB = pIMPLOTwindow.lock()->create_IMPLOTline(PhB_line_name);
    pIMPLOTline_PhC = pIMPLOTwindow.lock()->create_IMPLOTline(PhC_line_name);
    pIMPLOTline_i_d = pIMPLOTwindow.lock()->create_IMPLOTline(i_d_line_name);
    pIMPLOTline_i_q = pIMPLOTwindow.lock()->create_IMPLOTline(i_q_line_name);
}

void Motor::update() {
    pADC->get_values(ia, ib);
    
    Math::CLARKE_transformation(i_alpha, i_beta, ia, ib, false);
    Math::PARK_transformation(id, iq, i_alpha, i_beta, angle, false);
    
    pPI->calculate(id, iq, id_ref, iq_ref, vd, vq);
    pPI->dynamic_decoupling(speed, id, iq, vd, vq);
    pPI->VDQ_saturation(vd, vq);
    
    Math::PARK_transformation(v_alpha, v_beta, vd, vq, angle, true);
    
    //pKalman->update(v_alpha, v_beta, i_alpha, i_beta);
    
    pPWM->calculate_space_vector(v_alpha, v_beta);
    
    pIMPLOTline_ia.lock()->add_value(ia);
    pIMPLOTline_ib.lock()->add_value(ib);
    pIMPLOTline_i_alpha.lock()->add_value(i_alpha);
    pIMPLOTline_i_beta.lock()->add_value(i_beta);
    pIMPLOTline_i_d.lock()->add_value(id);
    pIMPLOTline_i_q.lock()->add_value(iq);
    
    //pIMPLOTline_speed.lock()->add_value(pKalman->get_speed());
    //pIMPLOTline_angle.lock()->add_value(pKalman->get_angle());
    
    pIMPLOTline_PhA.lock()->add_value(pPWM->get_PhA());
    pIMPLOTline_PhB.lock()->add_value(pPWM->get_PhB());
    pIMPLOTline_PhC.lock()->add_value(pPWM->get_PhC());
    
    //std::cout << pKalman->get_speed() << std::endl;
    //std::cout << pKalman->get_angle() << std::endl;
    std::cout << pPWM->get_PhA() << std::endl;
    std::cout << pPWM->get_PhB() << std::endl;
    std::cout << pPWM->get_PhC() << std::endl;
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

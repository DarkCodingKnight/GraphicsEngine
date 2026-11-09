#include "Motor.hpp"

namespace STM32 {

void Motor::start(Engine::IMGUIwindow* const _pIMGUIwindow_) {
    pIMGUIwindow = _pIMGUIwindow_;
    
    pADC = new ADC();
    pDRV8301 = new DRV8301(pADC, DRV_PARAM::GAIN_40V);
    pPWM = new PWM(pADC, MAX_MODULATION_INDEX, ENGINE_FREQUENCY::TEN_KHZ);
    
    adc_values = {};
    adc_values.PHASE_RESISTANCE = 0.105f;
    adc_values.LQ = 0.00012f;
    adc_values.MAG_FLUX = 0.01f;
    adc_values.TS = pPWM->get_TS();
    adc_values.GAIN = pDRV8301->get_gain();
    
    pADC->set_adc_values(&adc_values);
    
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
    
    Flux_values = {};
    Flux_values.PHASE_RESISTANCE = 0.105f;
    Flux_values.TS = pPWM->get_TS();
    Flux_values.OBSERVER_CUTOFF_FREQUENSY = 31.4f * 5.0f;
    Flux_values.SPEED_CUTOFF_FREQUENCY = 31.4f;
    Flux_values.LD = 0.00012f;
    Flux_values.LQ = 0.00012f;
    
    pFluxObserver = new FluxObserver(&Flux_values);
    
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
    pIMPLOTline_id = pIMPLOTwindow.lock()->create_IMPLOTline(id_line_name);
    pIMPLOTline_iq = pIMPLOTwindow.lock()->create_IMPLOTline(iq_line_name);
}

void Motor::update() {
    pADC->get_values(ia, ib, v_alpha, v_beta, speed, angle, pDRV8301->get_ia_offset(), pDRV8301->get_ib_offset());
    
    Math::CLARKE_transformation(i_alpha, i_beta, ia, ib, false);
    Math::PARK_transformation(id, iq, i_alpha, i_beta, angle, false);
    
    pPI->calculate(id, iq, id_ref, iq_ref, vd, vq);
    pPI->dynamic_decoupling(speed, id, iq, vd, vq);
    pPI->VDQ_saturation(vd, vq);
    
    Math::PARK_transformation(v_alpha, v_beta, vd, vq, angle, true);
    
    //pKalman->update(v_alpha, v_beta, i_alpha, i_beta);
    pFluxObserver->calculate(speed, angle, v_alpha, v_beta, i_alpha, i_beta);
    
    pPWM->calculate_space_vector(v_alpha, v_beta);
    
    pIMPLOTline_ia.lock()->add_value(ia);
    pIMPLOTline_ib.lock()->add_value(ib);
    pIMPLOTline_i_alpha.lock()->add_value(i_alpha);
    pIMPLOTline_i_beta.lock()->add_value(i_beta);
    pIMPLOTline_id.lock()->add_value(id);
    pIMPLOTline_iq.lock()->add_value(iq);
    
    pIMPLOTline_speed.lock()->add_value(speed);
    pIMPLOTline_angle.lock()->add_value(angle);
    
    pIMPLOTline_PhA.lock()->add_value(pPWM->get_PhA());
    pIMPLOTline_PhB.lock()->add_value(pPWM->get_PhB());
    pIMPLOTline_PhC.lock()->add_value(pPWM->get_PhC());
    
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Angle: " << angle << std::endl;
    
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
    
    delete pPWM;
    pPWM = nullptr;
    
    delete pPI;
    pPI = nullptr;
    
    delete pFluxObserver;
    pFluxObserver = nullptr;
    
    delete pKalman;
    pKalman = nullptr;
}

}

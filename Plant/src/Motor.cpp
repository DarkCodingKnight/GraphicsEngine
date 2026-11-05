#include "Motor.hpp"

namespace STM32 {

void Motor::start(Engine::IMGUIwindow* const _pIMGUIwindow_) {
    pIMGUIwindow = _pIMGUIwindow_;
    
    pIMPLOTwindow = pIMGUIwindow->create_ImPlot_window(graphic_name, line_name, x_name, y_name);
    
    pDRV8301 = new DRV8301(DRV_PARAM::GAIN_40V);
    pADC = new ADC(pDRV8301->get_gain());
}

void Motor::update() {
    pADC->get_values(ia, ib);
    
    pIMPLOTwindow.lock()->set_value(pADC->get_adc_value(ADC_RANKS::ADC_RANK_1));
}

Motor::~Motor() {
    pIMGUIwindow = nullptr;
    
    delete pDRV8301;
    pDRV8301 = nullptr;
    delete pADC;
    pADC = nullptr;
}

}

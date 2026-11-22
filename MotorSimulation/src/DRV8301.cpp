#include "DRV8301.hpp"

namespace STM32 {

void DRV8301::calibrate() {
    while (count < max_measurements) {
        ia_avg += pADC->get_adc_value(ADC_RANKS::ADC_RANK_1, 0.0f, 0.0f, 0.0f, 0.0f);
        ib_avg += pADC->get_adc_value(ADC_RANKS::ADC_RANK_2, 0.0f, 0.0f, 0.0f, 0.0f);

        count++;
    }

    ia_offset = 2048 - (ia_avg / max_measurements);
    ib_offset = 2048 - (ib_avg / max_measurements);
}

void DRV8301::set_gain(DRV_PARAM _gain_selector_) {
    switch (_gain_selector_) {
        case DRV_PARAM::GAIN_80V:
            gain = 0.0125f;
            break;
            
        case DRV_PARAM::GAIN_40V:
            gain = 0.025f;
            break;
            
        case DRV_PARAM::GAIN_20V:
            gain = 0.05f;
            break;
            
        case DRV_PARAM::GAIN_10V:
            gain = 0.1f;
            break;
            
        default:
            gain = 0.0f;
            break;
    }
}

}

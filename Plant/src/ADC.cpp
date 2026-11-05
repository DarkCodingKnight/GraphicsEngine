#include "ADC.hpp"

namespace STM32 {

void ADC::get_values(float& ia, float& ib) {
    float ia_count = get_adc_value(ADC_RANKS::ADC_RANK_1);
    float ib_count = get_adc_value(ADC_RANKS::ADC_RANK_2);
    
    ia = (float)((ia_count - 2048) * max_voltage) * gain / 4095;
    ib = (float)((ib_count - 2048) * max_voltage) * gain / 4095;
    
    battery_voltage = (float)(get_adc_value(ADC_RANKS::ADC_RANK_3) * max_voltage * inverse_resistance_divider_value / 4095);
}

uint32_t ADC::get_adc_value(ADC_RANKS rank) {
    if (rank == ADC_RANKS::ADC_RANK_1) {
        return 2048 + (rand() % 11 - 50);
    }
    else if (rank == ADC_RANKS::ADC_RANK_2) {
        return 2048 + (rand() % 11 - 50);
    }
    else if (rank == ADC_RANKS::ADC_RANK_3) {
        return 400 + (rand() % 11 - 15);
    }
}

}

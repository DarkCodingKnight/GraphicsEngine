#include "ADC.hpp"

namespace STM32 {

void ADC::get_values(float& ia,
                     float& ib,
                     float v_alpha,
                     float v_beta,
                     float speed,
                     float angle,
                     uint32_t ia_offset,
                     uint32_t ib_offset)
{
    float ia_count = get_adc_value(ADC_RANKS::ADC_RANK_1, ia, v_alpha, speed, angle);
    float ib_count = get_adc_value(ADC_RANKS::ADC_RANK_2, ib, v_beta, speed, angle);
    
    ia = (float)((ia_count - 2048) * max_voltage) * pADC_values->GAIN / 4095;
    ib = (float)((ib_count - 2048) * max_voltage) * pADC_values->GAIN / 4095;
    
    battery_voltage = (float)(get_adc_value(ADC_RANKS::ADC_RANK_3, 0.0f, 0.0f, 0.0f, 0.0f) * max_voltage * inverse_resistance_divider_value / 4095);
}

uint32_t ADC::get_adc_value(ADC_RANKS rank,
                            float current,
                            float voltage,
                            float speed,
                            float angle)
{
    if (rank == ADC_RANKS::ADC_RANK_1) {
        uint32_t value = (voltage - pADC_values->PHASE_RESISTANCE * current + speed * pADC_values->MAG_FLUX * sin(angle)) / pADC_values->LQ * pADC_values->TS;
        
        return value + (rand() % 11 - 5);
    }
    else if (rank == ADC_RANKS::ADC_RANK_2) {
        uint32_t value = (voltage - pADC_values->PHASE_RESISTANCE * current + speed * pADC_values->MAG_FLUX * sin(angle)) / pADC_values->LQ * pADC_values->TS;
        
        return value + (rand() % 11 - 5);
    }
    else if (rank == ADC_RANKS::ADC_RANK_3) {
        return 400 + (rand() % 101 - 50);
    }
}

}

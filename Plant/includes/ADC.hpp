#pragma once

#include <cstdint>
#include <cstdlib>

#include "Math/Math.hpp"

struct ADC_values {
    float PHASE_RESISTANCE = 0.0f;
    float LQ = 0.0f;
    float MAG_FLUX = 0.0f;
    float TS = 0.0f;
    float GAIN = 0.0f;
};

enum class ADC_RANKS {
    ADC_RANK_1,
    ADC_RANK_2,
    ADC_RANK_3
};

namespace STM32 {

class ADC {
public:
    ADC(const ADC& other) = delete;
    ADC(ADC&& other) = delete;
    ADC& operator=(const ADC& other) = delete;
    ADC& operator=(ADC&& other) = delete;
    
    ADC() = default;
    
    void set_adc_values(ADC_values* const _pADC_values_) { pADC_values = _pADC_values_; }
    
    void get_values(float& ia,
                    float& ib,
                    float v_alpha,
                    float v_beta,
                    float speed,
                    float angle);
    float get_battery_voltage() { return battery_voltage; }
    uint32_t get_adc_value(ADC_RANKS rank,
                           float current,
                           float voltage,
                           float speed,
                           float angle);
    
    ~ADC() {
        pADC_values = nullptr;
    }
    
private:
    ADC_values* pADC_values = nullptr;
    
    float battery_voltage = 0.0f;
    
    float max_voltage = 3.3f;
    float one_by_shunt_resistance = 2000.0f;
    float inverse_resistance_divider_value = 18.7272f;
};

}

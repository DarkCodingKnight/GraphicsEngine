#pragma once

#include <cstdint>
#include <cstdlib>

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
    
    ADC(float _gain_) { gain = _gain_; }
    
    void get_values(float& ia, float& ib);
    float get_battery_voltage() { return battery_voltage; }
    uint32_t get_adc_value(ADC_RANKS rank);
    
    ~ADC() = default;
    
private:
    float battery_voltage = 0.0f;
    float gain = 0.0f;
    
    float max_voltage = 3.3f;
    float one_by_shunt_resistance = 2000.0f;
    float inverse_resistance_divider_value = 18.7272f;
};

}

#pragma once

#include "ADC.hpp"
#include "Math/Math.hpp"

namespace STM32 {

enum class ENGINE_FREQUENCY {
    FIVE_KHZ,
    TEN_KHZ,
    TWENTY_KHZ,
    THIRTY_KHZ,
    FOURTY_KHZ
};

class PWM {
public:
    PWM(const PWM& other) = delete;
    PWM(PWM&& other) = delete;
    PWM& operator=(const PWM& other) = delete;
    PWM& operator=(PWM&& other) = delete;
    
    PWM(ADC* const _pADC_, float _max_modulation_index_, ENGINE_FREQUENCY _frequency_selector_) :
    pADC(_pADC_),
    MAX_MODULATION_INDEX(_max_modulation_index_)
    {
        set_frequency(_frequency_selector_);
    }
    
    void calculate_space_vector(float& v_alpha, float& v_beta);
    
    float get_TS() { return Ts; }
    float get_PhA() { return PhA; }
    float get_PhB() { return PhB; }
    float get_PhC() { return PhC; }
    
    void set_frequency(ENGINE_FREQUENCY _frequency_selector_);
    
    ~PWM() {
        pADC = nullptr;
    }
    
private:
    ADC* pADC = nullptr;
    
    float Ts = 0.0f;
    float MAX_MODULATION_INDEX = 0.0f;
    
    uint16_t PhA = 0.0f;
    uint16_t PhB = 0.0f;
    uint16_t PhC = 0.0f;
};

}

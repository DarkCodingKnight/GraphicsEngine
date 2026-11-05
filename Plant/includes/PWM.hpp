#pragma once

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
    
    PWM(ENGINE_FREQUENCY _frequency_selector_) { set_frequency(_frequency_selector_); }
    
    void set_frequency(ENGINE_FREQUENCY _frequency_selector_);
    
    ~PWM() = default;
    
private:
    float Ts = 0.0f;
};

}

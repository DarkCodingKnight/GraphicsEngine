#pragma once

#include "ADC.hpp"

#include "Kalman.hpp"

namespace STM32 {

class Motor {
public:
    Motor(const Motor& other) = delete;
    Motor(Motor&& other) = delete;
    Motor& operator=(const Motor& other) = delete;
    Motor& operator=(Motor&& other) = delete;
    
    Motor() = default;
    
    void start();
    
    ~Motor() = default;
    
private:
    ADC* pADC = nullptr;
};

}

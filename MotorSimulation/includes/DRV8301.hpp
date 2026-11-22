#pragma once

#include "ADC.hpp"

namespace STM32 {

enum class DRV_PARAM {
    GAIN_10V,
    GAIN_20V,
    GAIN_40V,
    GAIN_80V
};

class DRV8301 {
public:
    DRV8301(const DRV8301& other) = delete;
    DRV8301(DRV8301&& other) = delete;
    DRV8301& operator=(const DRV8301& other) = delete;
    DRV8301& operator=(DRV8301&& other) = delete;
    
    DRV8301(ADC* const _pADC_, DRV_PARAM _gain_selector_) :
        pADC(_pADC_)
    { set_gain(_gain_selector_); }
    
    void calibrate();
    float get_gain() { return gain; }
    uint32_t get_ia_offset() { return ia_offset; }
    uint32_t get_ib_offset() { return ib_offset; }
    
    ~DRV8301() {
        pADC = nullptr;
    }
    
private:
    ADC* pADC = nullptr;

    void set_gain(DRV_PARAM _gain_selector_);
    
    uint32_t ia_avg = 0;
    uint32_t ib_avg = 0;

    uint32_t ia_offset = 0;
    uint32_t ib_offset = 0;

    float gain = 0.0f;
    uint32_t count = 0;
    uint32_t max_measurements = 10000;
};

}

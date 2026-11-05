#pragma once

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
    
    DRV8301(DRV_PARAM _gain_selector_) { set_gain(_gain_selector_); }
    
    float get_gain() { return gain; }
    
    ~DRV8301() = default;
    
private:
    void set_gain(DRV_PARAM _gain_selector_);
    
    float gain = 0.0f;
};

}

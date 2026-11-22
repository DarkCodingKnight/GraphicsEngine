#pragma once

#include "ADC.hpp"
#include "Math/Math.hpp"

namespace STM32 {

struct PI_Current_Values {
    float DKP               = 0.0f;
    float DKI               = 0.0f;
    float QKP               = 0.0f;
    float QKI               = 0.0f;
    float PHASE_RESISTANCE  = 0.0f;
    float LD                = 0.0f;
    float LQ                = 0.0f;
    float MAG_FLUX          = 0.0f;
    float TS                = 0.0f;
};

class PI_controller {
public:
    PI_controller(const PI_controller& other) = delete;
    PI_controller(PI_controller&& other) = delete;
    PI_controller& operator=(const PI_controller& other) = delete;
    PI_controller& operator=(PI_controller&& other) = delete;
    
    PI_controller(PI_Current_Values* const _pCurrentValues_,
                  ADC* const _pADC_,
                  float max_modulation_index) :
    pCurrentValues(_pCurrentValues_),
    pADC(_pADC_),
    MAX_MODULATION_INDEX(max_modulation_index) {}
    
    void calculate(float& id,
                   float& iq,
                   float& id_ref,
                   float& iq_ref,
                   float& vd,
                   float& vq);
    void dynamic_decoupling(float& speed, float& id, float& iq, float& vd, float& vq);
    void VDQ_saturation(float& vd, float& vq);
    
    ~PI_controller() {
        pCurrentValues = nullptr;
        pADC = nullptr;
    }
    
private:
    PI_Current_Values* pCurrentValues = nullptr;
    ADC* pADC = nullptr;
    
    float MAX_MODULATION_INDEX = 0.0f;
    
    float voltage_saturation = 0.0f;
    float D_integral_value = 0.0f;
    float Q_integral_value = 0.0f;
};

}

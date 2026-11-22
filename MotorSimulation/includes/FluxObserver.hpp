#pragma once

#include <Math/Math.hpp>

namespace STM32 {
/*
float observer_cutoff_frequency = 31.4f * 5.0f;
    float speed_cutoff_frequency = 31.4f;
float Ld = 0.00012f;
float Lq = 0.00012f;

float flux_pm = 0.0f;
*/
struct Flux_observer_values {
    float PHASE_RESISTANCE = 0.0f;
    float TS = 0.0f;
    float OBSERVER_CUTOFF_FREQUENSY = 0.0f;
    float SPEED_CUTOFF_FREQUENCY = 0.0f;
    float LD = 0.0f;
    float LQ = 0.0f;
};

class FluxObserver {
public:
    FluxObserver(const FluxObserver& other) = delete;
    FluxObserver(FluxObserver&& other) = delete;
    FluxObserver& operator=(const FluxObserver& other) = delete;
    FluxObserver& operator=(FluxObserver&& other) = delete;
    
    FluxObserver(Flux_observer_values* const _pFlux_observer_values_) :
    pFlux_observer_values(_pFlux_observer_values_) {}
    
    void calculate(float& speed,
                   float& angle,
                   float v_alpha,
                   float v_beta,
                   float i_alpha,
                   float i_beta);
    
    ~FluxObserver() {
        pFlux_observer_values = nullptr;
    }
    
private:
    Flux_observer_values* pFlux_observer_values = nullptr;
    
    int8_t sign = 1;
    uint32_t position_current = 0;
    uint32_t position_previous = 0;
    
    float intermediate_alpha = 0.0f;
    float intermediate_beta = 0.0f;
};

}

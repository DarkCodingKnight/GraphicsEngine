#include "FluxObserver.hpp"

namespace STM32 {

void FluxObserver::calculate(float& speed,
                             float& angle,
                             float v_alpha,
                             float v_beta,
                             float i_alpha,
                             float i_beta)
{
    float e_alpha = v_alpha - i_alpha * pFlux_observer_values->PHASE_RESISTANCE;
    float e_beta = v_beta - i_beta * pFlux_observer_values->PHASE_RESISTANCE;
    float speed_e_modulus = fabsf(speed);

    if (speed < 0.0f) sign = -1;
    else sign = 1;
    
    if (speed_e_modulus < 0.1f) speed_e_modulus = 0.1f;

    float emf_gain = hypotf(speed, pFlux_observer_values->OBSERVER_CUTOFF_FREQUENSY) / speed_e_modulus;
    float emf_mag = hypotf(e_alpha, e_beta) * emf_gain;
    float emf_angle_comp = atan2f(pFlux_observer_values->OBSERVER_CUTOFF_FREQUENSY, speed_e_modulus) + pFlux_observer_values->TS * speed_e_modulus;
    
    float emf_angle = atan2f(e_alpha, e_beta) + emf_angle_comp * sign;
    float e_alpha_compensated = emf_mag * sinf(emf_angle);
    float e_beta_compensated = emf_mag * cosf(emf_angle);

    intermediate_alpha = pFlux_observer_values->TS * (e_alpha_compensated - intermediate_alpha * pFlux_observer_values->OBSERVER_CUTOFF_FREQUENSY) + intermediate_alpha;
    intermediate_beta = pFlux_observer_values->TS * (e_beta_compensated - intermediate_beta * pFlux_observer_values->OBSERVER_CUTOFF_FREQUENSY) + intermediate_beta;

    angle = atan2f(intermediate_beta - i_beta * pFlux_observer_values->LQ, intermediate_alpha - i_alpha * pFlux_observer_values->LQ);

    position_current = (uint32_t)((int)(4294967296 / TWO_PI * angle));

    int32_t delta_pos = (int32_t)(position_current - position_previous);
    float speed_raw = (float)delta_pos * (TWO_PI / 4294967296.0f) / pFlux_observer_values->TS;
    float alpha_f = pFlux_observer_values->SPEED_CUTOFF_FREQUENCY * pFlux_observer_values->TS;
    if (alpha_f > 1.0f) alpha_f = 1.0f;
    speed = speed + alpha_f * (speed_raw - speed);
    
    position_previous = position_current;
}

}

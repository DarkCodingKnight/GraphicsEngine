#include "PWM.hpp"

namespace STM32 {

void PWM::calculate_space_vector(float& v_alpha, float& v_beta) {
    float Va = v_alpha;
    float Vb = v_beta * SQRT3_BY_2 - 0.5f * v_alpha;
    float Vc = - v_beta * SQRT3_BY_2 - 0.5f * v_alpha;

    float max = Va;
    if (Vb > max) max = Vb;
    if (Vc > max) max = Vc;

    float min = Va;
    if (Vb < min) min = Vb;
    if (Vc < min) min = Vc;

    float third_harmonic = 0.5f * (max + min);

    float battery_voltage = pADC->get_battery_voltage();

    float A = (Va - third_harmonic) / battery_voltage + MAX_MODULATION_INDEX / 2.0f;
    float B = (Vb - third_harmonic) / battery_voltage + MAX_MODULATION_INDEX / 2.0f;
    float C = (Vc - third_harmonic) / battery_voltage + MAX_MODULATION_INDEX / 2.0f;

    if (A < 0.0f) A = 0.0f;
    if (B < 0.0f) B = 0.0f;
    if (C < 0.0f) C = 0.0f;

    PhA = static_cast<uint16_t>(350.0f * A);
    PhB = static_cast<uint16_t>(350.0f * B);
    PhC = static_cast<uint16_t>(350.0f * C);
}

void PWM::set_frequency(ENGINE_FREQUENCY _frequency_selector_) {
    switch (_frequency_selector_) {
        case ENGINE_FREQUENCY::FIVE_KHZ:
            Ts = 0.0002f;
            break;
            
        case ENGINE_FREQUENCY::TEN_KHZ:
            Ts = 0.0001f;
            break;
            
        case ENGINE_FREQUENCY::TWENTY_KHZ:
            Ts = 0.00005f;
            break;
            
        case ENGINE_FREQUENCY::THIRTY_KHZ:
            Ts = 0.00003333f;
            break;
            
        case ENGINE_FREQUENCY::FOURTY_KHZ:
            Ts = 0.000025f;
            break;
            
        default:
            Ts = 0.0f;
            break;
    }
}

}

#include "PWM.hpp"

namespace STM32 {

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

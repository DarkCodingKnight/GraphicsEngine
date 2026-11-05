#include "PI.hpp"

namespace STM32 {

void PI_controller::calculate(float& id,
                              float& iq,
                              float& id_ref,
                              float& iq_ref,
                              float& vd,
                              float& vq)
{
    voltage_saturation = pADC->get_battery_voltage() * MAX_MODULATION_INDEX * ONE_BY_SQRT3;

    static uint8_t D_clamp = 1;
    float D_error = id_ref - id;

    vd = pCurrentValues->DKI * D_integral_value + pCurrentValues->DKP * D_error;

    if (vd >= voltage_saturation || vd <= - voltage_saturation) D_clamp = 0;
    else D_clamp = 1;

    if (vd > voltage_saturation) vd = voltage_saturation;
    if (vd < - voltage_saturation) vd = - voltage_saturation;

    D_integral_value = D_integral_value + pCurrentValues->TS * D_error * D_clamp;

    static uint8_t Q_clamp = 1;
    float Q_error = iq_ref - iq;

    vq = pCurrentValues->QKI * Q_integral_value + pCurrentValues->QKP * Q_error;

    if (vq >= voltage_saturation || vq <= - voltage_saturation) Q_clamp = 0;
    else Q_clamp = 1;

    if (vq > voltage_saturation) vq = voltage_saturation;
    if (vq < - voltage_saturation) vq = - voltage_saturation;

    Q_integral_value = Q_integral_value + pCurrentValues->TS * Q_error * Q_clamp;
}

void PI_controller::dynamic_decoupling(float& speed, float& id, float& iq, float& vd, float& vq)
{
    float vd_ff = - speed * id * pCurrentValues->LD * 0.866f;
    if (vd_ff >= voltage_saturation) vd_ff = voltage_saturation;
    else if (vd_ff <= - voltage_saturation) vd_ff = - voltage_saturation;

    float vq_ff = speed * (pCurrentValues->LQ * iq + pCurrentValues->MAG_FLUX) * 0.866f;
    if (vq_ff >= voltage_saturation) vq_ff = voltage_saturation;
    else if (vq_ff <= - voltage_saturation) vq_ff = - voltage_saturation;

    vd += vd_ff;
    vq += vq_ff;
}

void PI_controller::VDQ_saturation(float& vd, float& vq)
{
    float vs_unsat = hypotf(vd, vq);

    if (vs_unsat > voltage_saturation)
    {
        vd = vd / vs_unsat * voltage_saturation;
        vq = vq / vs_unsat * voltage_saturation;
    }
}


}

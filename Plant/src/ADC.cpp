#include "ADC.hpp"

namespace STM32 {

void ADC::get_values(float& ia, float& ib, float v_alpha, float v_beta,
                     float speed, float angle,
                     uint32_t ia_offset, uint32_t ib_offset) // offsets теперь по ссылке
{
    // 1. Физическая симуляция мотора (метод Эйлера)
    // Считаем изменение тока за шаг времени TS (dI = V/L * dt)

    v_alpha = 5.0f;
    v_beta = 0.0f;

    // Ось Alpha
    float bemf_alpha = speed * pADC_values->MAG_FLUX * sin(angle);
    float di_alpha = (v_alpha - pADC_values->PHASE_RESISTANCE * i_alpha_state + bemf_alpha)
                     / pADC_values->LQ * pADC_values->TS;

    // Ось Beta
    float bemf_beta = -speed * pADC_values->MAG_FLUX * cos(angle); // Здесь косинус и минус!
    float di_beta = (v_beta - pADC_values->PHASE_RESISTANCE * i_beta_state + bemf_beta)
                    / pADC_values->LQ * pADC_values->TS;

    // Обновляем "состояние" тока мотора (накапливаем)
    i_alpha_state += di_alpha;
    i_beta_state  += di_beta;

    // 2. Преобразуем токи из Alpha/Beta обратно в фазы A и B (Inverse Clarke)
    float i_a = i_alpha_state;
    float i_b = -0.5f * i_alpha_state + 0.866025f * i_beta_state; // sqrt(3)/2 = 0.866

    // 3. Переводим физический ток (Амперы) в тики АЦП (0-4095)
    // Предположим, что max_current = 50А (ток, при котором АЦП выдает 4095)
    float max_measurable_current = 50.0f;

    // 2048 — это нулевой ток (середина шкалы)
    int32_t adc_a_raw = (int32_t)((i_a / max_measurable_current) * 2048.0f) + 2048;
    int32_t adc_b_raw = (int32_t)((i_b / max_measurable_current) * 2048.0f) + 2048;

    // 4. Добавляем шум АЦП (симуляция помех)
    adc_a_raw += (rand() % 11 - 5);
    adc_b_raw += (rand() % 11 - 5);

    // Ограничиваем значения в пределах 0-4095 (как в реальном АЦП)
    if (adc_a_raw > 4095) adc_a_raw = 4095;
    if (adc_a_raw < 0) adc_a_raw = 0;
    if (adc_b_raw > 4095) adc_b_raw = 4095;
    if (adc_b_raw < 0) adc_b_raw = 0;

    // 5. Возвращаем "сырые" значения АЦП для FOC алгоритма
    ia_count = (uint32_t)adc_a_raw;
    ib_count = (uint32_t)adc_b_raw;

    // 6. Пересчитываем обратно в физические амперы (как в твоем оригинальном коде)
    ia = (float)((adc_a_raw - 2048) * max_voltage) * pADC_values->GAIN / 4095.0f;
    ib = (float)((adc_b_raw - 2048) * max_voltage) * pADC_values->GAIN / 4095.0f;

    // Напряжение батареи (симуляция)
    uint32_t bat_adc = 2500 + (rand() % 101 - 50); // Допустим, батарея 24В дает 2500 тиков
    battery_voltage = (float)(bat_adc * max_voltage * inverse_resistance_divider_value / 4095.0f);
}

uint32_t ADC::get_adc_value(ADC_RANKS rank,
                            float current,
                            float voltage,
                            float speed,
                            float angle)
{
    if (rank == ADC_RANKS::ADC_RANK_1) {
        uint32_t value = (voltage - pADC_values->PHASE_RESISTANCE * current + speed * pADC_values->MAG_FLUX * sin(angle)) / pADC_values->LQ * pADC_values->TS;
        
        return value + (rand() % 11 - 5);
    }
    else if (rank == ADC_RANKS::ADC_RANK_2) {
        uint32_t value = (voltage - pADC_values->PHASE_RESISTANCE * current + speed * pADC_values->MAG_FLUX * sin(angle)) / pADC_values->LQ * pADC_values->TS;
        
        return value + (rand() % 11 - 5);
    }
    else if (rank == ADC_RANKS::ADC_RANK_3) {
        return 400 + (rand() % 101 - 50);
    }
}

}

#pragma once

#include <cstdint>
#include <cstdio>
#include <cmath>

namespace STM32 {

#define PI                                    3.14159265f
#define PI_BY_180                             0.0174532925f
#define PI_BY_3                               1.04719755f
#define ONE_BY_SQRT3                          0.5773502691896257f
#define SQRT3                                 1.7320508075f
#define INVERSE_PI_BY_180                     57.295779513f
#define SQRT3_BY_2                            0.8660254037844f
#define TWO_PI                                6.28318530717f

class Math {
public:
    Math() = default;
    Math(const Math& other) = delete;
    Math(Math&& other) = delete;
    Math& operator=(const Math& other) = delete;
    Math& operator=(Math&& other) = delete;
    ~Math() = default;

    static void PARK_CLARK_transformations(float& first_value,
                                           float& second_value,
                                           float alpha,
                                           float beta,
                                           float position)
    {
        float CLARKE_alpha = 0.0f;
        float CLARKE_beta = 0.0f;

        CLARKE_transformation(CLARKE_alpha, CLARKE_beta, alpha, beta, false);
        PARK_transformation(first_value, second_value, CLARKE_alpha, CLARKE_beta, position, false);
    }

    static void CLARKE_transformation(float& first_value,
                                      float& second_value,
                                      float alpha,
                                      float beta,
                                      bool is_inverse)
    {
        first_value = alpha;

        is_inverse ? second_value = alpha * -0.5f + (SQRT3 / 2.0f) * beta : second_value = (alpha + 2.0f * beta) * ONE_BY_SQRT3;
    }

    static void PARK_transformation(float& first_value,
                                    float& second_value,
                                    float alpha,
                                    float beta,
                                    float position,
                                    bool is_inverse)
    {
        float sin_position = sinf(position);
        float cos_position = cosf(position);

        is_inverse ? first_value = (cos_position * alpha) - (sin_position * beta) : first_value = (cos_position * alpha) + (sin_position * beta);
        is_inverse ? second_value = (cos_position * beta) + (sin_position * alpha) : second_value = (cos_position * beta) - (sin_position * alpha);
    }
};

}

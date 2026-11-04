#include "Kalman.hpp"

namespace STM32 {

void Kalman::init() {
    // Вектор состояния
    // i_alpha, i_beta, speed, angle
    x = glm::vec4(0.0f);
    
    // Вектор управления
    u = glm::vec2(0.0f);
    
    // Матрица управления
    B = glm::mat2x4(0.0f);
    B[0][0] = pKalmanValues->TS / pKalmanValues->LQ;
    B[1][1] = pKalmanValues->TS / pKalmanValues->LQ;
    
    // матрица перехода состояния
    F = glm::mat4x4(0.0f);
    
    // Транспонированная F
    Ft = glm::mat4x4(0.0f);
    
    // Параметры для матрицы перехода состояния
    c1 = pKalmanValues->TS * pKalmanValues->PHASE_RESISTANCE / pKalmanValues->LQ;
    c2 = pKalmanValues->TS * pKalmanValues->MAG_FLUX / pKalmanValues->LQ;
    
    // Матрица ковариации ошибки
    P = glm::mat4x4(0.0f);
    P[0][0] = 0.01f;
    P[1][1] = 0.01f;
    P[2][2] = 100.0f;
    P[3][3] = 1000.0f;
    
    // Матрица ковариации шума процесса
    Q = glm::mat4x4(0.0f);
    Q[0][0] = 1e-4f;
    Q[1][1] = 1e-4f;
    Q[2][2] = 1e-2f;
    Q[3][3] = 1e-6f;
    
    // Вектор измерения
    z = glm::vec2(0.0f);
    
    // Матрица наблюдателя
    H = glm::mat4x2(0.0f);
    H[0][0] = 1.0f;
    H[1][1] = 1.0f;
    
    // Транспонированная H
    Ht = glm::mat2x4(0.0f);
    Ht = glm::transpose(H);
    
    // Матрица ковариации шума
    R = glm::mat2x2(0.0f);
    R[0][0] = 0.05f * 0.05f;
    R[1][1] = 0.05f * 0.05f;
    
    // Матрица (коэффициент усиления Калмана)
    K = glm::mat2x4(0.0f);
}

void Kalman::update(float v_alpha,
                    float v_beta,
                    float i_alpha,
                    float i_beta)
{
    if (!pKalmanValues) return;
    
    // --- Предсказание ---
    // Обновляем вектора состояния и управления
    u = { v_alpha, v_beta };
    
    speed = x[2];
    angle = x[3];
    
    // Обновляем матрицу состояния
    F = glm::mat4x4(0.0f);
    F[0][0] = 1.0f - c1;                    // d(i_alpha) / d(i_alpha)
    F[0][1] = 0.0f;                         // d(i_alpha) / d(i_beta)
    F[0][2] = c2 * sin(angle);              // d(i_alpha) / d(speed)
    F[0][3] = c2 * speed * cos(angle);      // d(i_alpha) / d(angle)
    F[1][0] = 0.0f;
    F[1][1] = 1.0f - c1;
    F[1][2] = -c2 * cos(angle);             // d(i_beta) / d(speed)
    F[1][3] = c2 * speed * sin(angle);      // d(i_beta) / d(angle)
    F[2][0] = 0.0f;
    F[2][1] = 0.0f;
    F[2][2] = 1.0f;
    F[2][3] = 0.0f;
    F[3][0] = 0.0f;
    F[3][1] = 0.0f;
    F[3][2] = pKalmanValues->TS;            // d(angle) / d(speed)
    F[3][3] = 1.0f;
    
    // Обновляем транспонированную F
    Ft = glm::transpose(F);
    
    // Рассчитываем предсказание
    prediction();
    
    // --- Корректировка ---
    // Обновление вектора измерения
    z = { i_alpha, i_beta };
    
    // Рассчитываем корректировку
    correction();
}

void Kalman::prediction() {
    // Рассчитываем вектор состояния
    x = (F * x) + (B * u);
    
    // Рассчитываем ковариационную матрицу ошибки
    P = (F * P * Ft) + Q;
}

void Kalman::correction() {
    // Рассчет коэффициента усиления Калмана
    glm::mat2x2 S = (H * P * Ht) + R;
    
    K = (P * Ht) * inverse2x2(S);
    
    // Рассчет вектора состояния
    x = x + K * (z - H * x);
    
    // Рассчитываем скорректированную матрицу ошибки
    P = P - K * (H * P);
}

glm::mat2x2 inverse2x2(glm::mat2x2& M) {
    float det = M[0][0] * M[1][1] - M[0][1] * M[1][0];
    if (fabsf(det) < 1e-12f) return glm::mat2x2(0.0f);
    float inv_det = 1.0f / det;
    
    glm::mat2x2 R = glm::mat2x2(0.0f);
    R[0][0] =  M[1][1] * inv_det;
    R[0][1] = -M[0][1] * inv_det;
    R[1][0] =  M[1][0] * inv_det;
    R[1][1] = -M[0][0] * inv_det;
    
    return R;
}

}

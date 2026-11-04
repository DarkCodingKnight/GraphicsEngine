#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace STM32 {

struct KalmanValues {
    float PHASE_RESISTANCE = 0.0f;
    float LQ = 0.0f;
    float MAG_FLUX = 0.0f;
    float TS = 0.0f;
};

class Kalman {
public:
    Kalman(const Kalman& other) = delete;
    Kalman(Kalman&& other) = delete;
    Kalman& operator=(const Kalman& other) = delete;
    Kalman& operator=(Kalman&& other) = delete;
    
    Kalman() = default;
    
    void set_KalmanValues(KalmanValues* const _pKalmanValues_) { pKalmanValues = _pKalmanValues_; }
    
    void init();
    void update(float v_alpha,
                float v_beta,
                float i_alpha,
                float i_beta);
    
    ~Kalman() {
        pKalmanValues = nullptr;
    }
    
private:
    void prediction();
    void correction();
    glm::mat2x2 inverse2x2(glm::mat2x2& M);
    
    KalmanValues* pKalmanValues = nullptr;
    
    glm::mat2x4 B;       // матрица управления
    glm::mat4x4 F;       // матрица перехода состояния
    glm::mat4x4 Ft;      // транспонированная F
    glm::mat4x4 P;       // матрица ковариации ошибки
    glm::mat4x4 Q;       // матрица ковариации шума процесса
    
    glm::mat4x2 H;       // матрица наблюдателя
    glm::mat2x4 Ht;      // транспонированная H
    glm::mat2x2 R;       // матрица ковариации шума
    glm::mat2x4 K;       // матрица (коэффициент усиления Калмана)
    
    glm::vec4 x;         // вектор состояния
    glm::vec2 u;         // вектор управления
    glm::vec2 z;         // Вектор измерения
    
    float speed = 0.0f;
    float angle = 0.0f;
    
    float c1 = 0.0f;
    float c2 = 0.0f;
};

}

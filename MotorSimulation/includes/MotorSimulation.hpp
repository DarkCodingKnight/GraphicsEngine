#pragma once

#include "Motor.hpp"

namespace STM32 {

class MotorSimulation {
public:
    MotorSimulation();
    MotorSimulation(const MotorSimulation& other) = delete;
    MotorSimulation(MotorSimulation&& other) = delete;
    MotorSimulation& operator=(const MotorSimulation& other) = delete;
    MotorSimulation& operator=(MotorSimulation&& other) = delete;

    void init();
    void start();

    ~MotorSimulation();

private:
    std::unique_ptr<Engine::GraphicsApplication> pGraphicsApplication;
    std::weak_ptr<Engine::GraphicsObjectPool> pObjectPool;
    std::weak_ptr<Engine::IMGUImanager> pIMGUImanager;
    std::weak_ptr<Engine::AudioManager> pAudioManager;
    Motor* pMotor = nullptr;

    const char* APPLICATION_NAME = "Motor simulation";
    const uint16_t WINDOW_WIDTH = 1000;
    const uint16_t WINDOW_HEIGHT = 700;
    const uint16_t MAX_FRAMES_IN_FLIGHT = 2;
    const bool ENABLE_VALIDATION_LAYERS = true;
    const std::string VERTEX_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/shaders/vert.spv";
    const std::string FRAGMENT_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/shaders/frag.spv";
};

}
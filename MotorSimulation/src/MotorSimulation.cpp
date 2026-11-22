#include "MotorSimulation.hpp"

namespace STM32 {

MotorSimulation::MotorSimulation() { init(); }

void MotorSimulation::init() {
    Engine::GraphicsApplicationInfo applicationInfo{};
    applicationInfo.application_name = APPLICATION_NAME;
    applicationInfo.enable_validation_Layers = ENABLE_VALIDATION_LAYERS;
    applicationInfo.max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;
    applicationInfo.window_width = WINDOW_WIDTH;
    applicationInfo.window_height = WINDOW_HEIGHT;
    pGraphicsApplication = std::make_unique<Engine::GraphicsApplication>(&applicationInfo);

    Engine::ObjectPoolInfo objectPoolInfo{};
    objectPoolInfo.fragmentPath = FRAGMENT_PATH;
    objectPoolInfo.vertexPath = VERTEX_PATH;
    objectPoolInfo.max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;
    pObjectPool = pGraphicsApplication->createGraphicsObjectPool(&objectPoolInfo);

    //pIMGUImanager = pGraphicsApplication->create_IMGUI_manager();
    pAudioManager = pGraphicsApplication->create_audio_manager();

    pMotor = new Motor(pObjectPool);
}

void MotorSimulation::start() {
    pAudioManager.lock()->play_open_sound();

    while (pGraphicsApplication->updateGraphicsApplication()) {
        pMotor->draw();
        pMotor->update();

        static bool is_last_pressed = false;
        bool is_pressed = pGraphicsApplication->getMouseKyPressed(0);
        if (is_pressed) {
            if (!is_last_pressed) pAudioManager.lock()->play_button_sound();
            is_last_pressed = true;
        }
        else is_last_pressed = false;
    }
}

MotorSimulation::~MotorSimulation() {
    delete pMotor;
    pMotor = nullptr;
}

}
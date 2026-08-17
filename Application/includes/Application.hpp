#pragma once

#include "GraphicsApplication.hpp"

namespace EngineEditor {

class Application {
public:
    Application() = delete;
    Application(const Application& other) = delete;
    Application(Application&& other) = delete;
    Application& operator=(const Application& other) = delete;
    Application& operator=(Application&& other) = delete;
    
    Application(bool validation_layers) : enable_validation_layers(validation_layers) {};
    
    void start();
    
    ~Application() { pGraphicsApplication.reset(); };
    
private:
    void mainLoop();
    
    std::unique_ptr<Engine::GraphicsApplication> pGraphicsApplication;
    std::weak_ptr<Engine::GraphicsObjectPool> pObjectPool;
    std::vector<std::weak_ptr<Engine::GraphicsObject>> pObjects;
    
    bool enable_validation_layers = false;
    
    const char* APPLICATION_NAME = "Hello world!";
    const uint16_t MAX_FRAMES_IN_FLIGHT = 2;
    const uint16_t WINDOW_WIDTH = 500;
    const uint16_t WINDOW_HEIGHT = 500;
    const std::string VERTEX_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/shaders/vert.spv";
    const std::string FRAGMENT_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/shaders/frag.spv";
    const char* TEXTURE_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/VikingRoom/VikingRoomTexture.png";
    const char* MODEL_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/VikingRoom/VikingRoomModel.obj";
};

}

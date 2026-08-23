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
    
    std::weak_ptr<Engine::GraphicsObject> pTree;
    std::weak_ptr<Engine::GraphicsObject> pDesert;
    std::weak_ptr<Engine::GraphicsObject> pVietnam;
    
    bool enable_validation_layers = false;
    
    const char* APPLICATION_NAME = "Hello world!";
    const uint16_t MAX_FRAMES_IN_FLIGHT = 2;
    const uint16_t WINDOW_WIDTH = 500;
    const uint16_t WINDOW_HEIGHT = 500;
    const std::string VERTEX_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/shaders/vert.spv";
    const std::string FRAGMENT_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/shaders/frag.spv";
    
    
    const char* TREE_TEXTURE_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/Tree/Textures/TreeTextureBaseColor.png";
    const char* TREE_MODEL_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/Tree/Model/TreeModel.obj";
    
    const char* DESERT_TEXTURE_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/Desert/DesertTexture.png";
    const char* DESERT_MODEL_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/Desert/DesertModel.obj";
    
    const char* VIETNAM_TEXTURE_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/Vietnam/VietnamTexture.png";
    const char* VIETNAM_MODEL_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/Vietnam/VietnamModel.obj";
    
    //const char* TEXTURE_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/VikingRoom/VikingRoomTexture.png";
    //const char* MODEL_PATH = "/Users/user/Projects/GameEngine/GraphicsEngine/models/VikingRoom/VikingRoomModel.obj";
};

}

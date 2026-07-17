//#define RELEASE

#include <iostream>

#include "ObjectPool.hpp"
#include "ConsoleText.hpp"

#ifdef RELEASE
bool enableValidationLayers = false;
#else
bool enableValidationLayers = true;
#endif

namespace EngineEditor {
    
class Editor {
public:
    Editor() {};
    
    void StartEngine() {
        pGraphicsApplication = new Engine::GraphicsApplication(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
        pObjectPool = new Engine::ObjectPool(pGraphicsApplication,
                                             TITLE,
                                             enableValidationLayers,
                                             MAX_FRAMES_IN_FLIGHT,
                                             vertexPath,
                                             fragmentPath);
        
        pObjectPool->createObject(vertices, indices);
        
        mainLoop();
        
        clenup();
    }
    
    void mainLoop() {
        while(pObjectPool->updateObjects()) {};
    }
    
    void clenup() {
        delete pObjectPool;
        delete pGraphicsApplication;
    }
    
    ~Editor() {};
    
private:
    
    Engine::GraphicsApplication* pGraphicsApplication;
    Engine::ObjectPool* pObjectPool;
    
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 500;
    const char* TITLE = "GLFW window";
    
    const std::vector<Engine::Vertex> vertices = {
        {{-0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
        {{ 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
        {{ 0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
        {{-0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }}
    };
    
    const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
    
    const std::string vertexPath = "/Users/user/Projects/GraphicsEngine/Engine/shaders/vert.spv";
    const std::string fragmentPath = "/Users/user/Projects/GraphicsEngine/Engine/shaders/frag.spv";
    
    //"/../../../GraphicsEngine/Engine/shaders/vert.spv"
    //"/../../../GraphicsEngine/Engine/shaders/frag.spv"
};

}

int main() {
    EngineEditor::Editor editor = EngineEditor::Editor();
    
    try {
        editor.StartEngine();
    } catch (const std::exception& e) {
        std::string message = "Engine error! " + static_cast<std::string>(e.what());
        ConsoleText::printError(message, "Engine editor");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

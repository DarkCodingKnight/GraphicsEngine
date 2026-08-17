#include "Application.hpp"

namespace EngineEditor {

void Application::start() {
    Engine::GraphicsApplicationInfo applicationInfo{};
    applicationInfo.application_name = APPLICATION_NAME;
    applicationInfo.enable_validation_Layers = enable_validation_layers;
    applicationInfo.max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;
    applicationInfo.window_width = WINDOW_WIDTH;
    applicationInfo.window_height = WINDOW_HEIGHT;
    
    pGraphicsApplication = std::make_unique<Engine::GraphicsApplication>(&applicationInfo);
    
    Engine::ObjectPoolInfo objectPoolInfo{};
    objectPoolInfo.fragmentPath = FRAGMENT_PATH;
    objectPoolInfo.vertexPath = VERTEX_PATH;
    objectPoolInfo.max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;
    
    std::weak_ptr<Engine::GraphicsObjectPool> pObjectPool = pGraphicsApplication->createGraphicsObjectPool(&objectPoolInfo);
    
    
    auto pObject = pObjectPool.lock()->createGraphicsObject(MODEL_PATH, TEXTURE_PATH);
    pObjects.push_back(pObject);
    
    mainLoop();
}

void Application::mainLoop() {
    while (pGraphicsApplication->updateGraphicsApplication()) {
        for (auto pObject : pObjects) {
            pObject.lock()->draw();
            
            //pObject.lock()->translate(0.005f, 0.0f, 0.0f);
            //pObject.lock()->rotate(0.005f, true, true, true);
            pObject.lock()->scale(0.01f, 0.01f, 0.01f);
        }
    }
}

}

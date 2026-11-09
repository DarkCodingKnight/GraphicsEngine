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
    
    //pTree = pObjectPool.lock()->createGraphicsObject(TREE_MODEL_PATH, TREE_TEXTURE_PATH, "Tree");
    //pTree.lock()->scale(-0.995f, -0.995f, -0.995f);
    //pDesert = pObjectPool.lock()->createGraphicsObject(DESERT_MODEL_PATH, DESERT_TEXTURE_PATH, "Desert");
    
    pSaw = pObjectPool.lock()->createGraphicsObject(SAW_MODEL_PATH, SAW_TEXTURE_PATH, "Saw");
    
    // Motor initializing
    pMotor = new STM32::Motor();
    pMotor->start(pGraphicsApplication->get_IMGUI());
    
    mainLoop();
}

void Application::mainLoop() {
    while (pGraphicsApplication->updateGraphicsApplication()) {
        //pArrow.lock()->draw();
        //pLowPoyTree.lock()->draw();
        //pDesert.lock()->draw();
        //pTree.lock()->draw();
        
        pSaw.lock()->draw();
        
        pSaw.lock()->rotate(0.01f, false, false, true);
        
        pMotor->update();
    }
}


//if (pGraphicsApplication->getKeyPressed(49)) pTree.lock()->scale(-0.00005f, -0.00005f, -0.00005f);
//if (pGraphicsApplication->getKeyPressed(50)) pTree.lock()->scale(0.00005f, 0.00005f, 0.00005f);
//
//if (pGraphicsApplication->getKeyPressed(51)) pTree.lock()->translate(3.0f, 0.0f, 0.0f);
//if (pGraphicsApplication->getKeyPressed(52)) pTree.lock()->translate(-3.0f, 0.0f, 0.0f);
//
//if (pGraphicsApplication->getKeyPressed(53)) pTree.lock()->translate(0.0f, 0.0f, 3.0f);
//if (pGraphicsApplication->getKeyPressed(54)) pTree.lock()->translate(0.0f, 0.0f, -3.0f);
//
//if (pGraphicsApplication->getKeyPressed(55)) pTree.lock()->translate(0.0f, 2.0f, 0.0f);
//if (pGraphicsApplication->getKeyPressed(56)) pTree.lock()->translate(0.0f, -2.0f, 0.0f);

}

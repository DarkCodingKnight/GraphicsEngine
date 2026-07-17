#include "ObjectPool.hpp"

namespace Engine {

ObjectPool::ObjectPool(GraphicsApplication* const pApp,
                       const char* applicationName,
                       const bool validLayers,
                       const uint32_t frames_in_flight,
                       const std::string& vertexPath,
                       const std::string& fragmentPath) :
validationLayers(validLayers), max_frames_in_flight(frames_in_flight)
{
    if (pApp != nullptr) {
        pGraphicsApplication = pApp;
    }
    else {
        ConsoleText::printError("Graphics application is nullptr!", "ObjectPool");
        std::exit(0);
    }
    
    initObjectPool(applicationName, vertexPath, fragmentPath);
    pGraphicsApplication->getWindowPtr()->setUpdateObjects(pRenderer);
}

void ObjectPool::initObjectPool(const char* appName,
                                const std::string& vertexPath,
                                const std::string& fragmentPath)
{
    pAPImanager = new APImanager(validationLayers,
                                 pGraphicsApplication->getWindowPtr()->getGLFWwindowPtr(),
                                 max_frames_in_flight);
    
    pAPImanager->initAPImanager(appName);
    
    std::vector<char> vertCode{};
    std::vector<char> fragCode{};
    
    std::optional<std::vector<char>> vertEnableCode = pGraphicsApplication->getResourceManagerPtr()->readFile(vertexPath);
    if (vertEnableCode.has_value()) vertCode = vertEnableCode.value();
    
    std::optional<std::vector<char>> fragEnableCode = pGraphicsApplication->getResourceManagerPtr()->readFile(fragmentPath);
    if (fragEnableCode.has_value()) fragCode = fragEnableCode.value();
    
    pPipeline = new Pipeline(pAPImanager,
                         pGraphicsApplication->getWindowPtr()->getGLFWwindowPtr(),
                         vertCode,
                         fragCode);
    
    pRenderer = new Renderer(pAPImanager,
                             max_frames_in_flight,
                             pGraphicsApplication->getWindowPtr()->getGLFWwindowPtr());
}

void ObjectPool::createObject(const std::vector<Vertex>& vertices,
                              const std::vector<uint16_t>& indices)
{
    Object* pObject = new Object(pAPImanager, pRenderer, vertices, indices);
    
    pObjects.push_back(pObject);
}

bool ObjectPool::updateObjects() {
    glfwPollEvents();
    
    for (auto object : pObjects) {
        object->draw(pPipeline);
    }
    
    if (pGraphicsApplication->getWindowPtr()->isWindowClose()) return false;
    else return true;
}

ObjectPool::~ObjectPool() {
    delete pRenderer;
    delete pPipeline;
    
    for (auto object : pObjects) {
        delete object;
    }
    
    delete pAPImanager;
}

}

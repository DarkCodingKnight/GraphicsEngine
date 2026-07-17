#pragma once

#include <glm/glm.hpp>

#include "GraphicsApplication.hpp"
#include "ConsoleText.hpp"
#include "Object.hpp"

namespace Engine {

class ObjectPool
{
public:
    ObjectPool() = delete;
    ObjectPool(GraphicsApplication* const pApp,
               const char* applicationName,
               const bool validLayers,
               const uint32_t frames_in_flight,
               const std::string& vertexPath,
               const std::string& fragmentPath);
    
    void createObject(const std::vector<Vertex>& vertices,
                      const std::vector<uint16_t>& indices);
    bool updateObjects();
    
    ~ObjectPool();
    
private:
    void initObjectPool(const char* appName,
                        const std::string& vertexPath,
                        const std::string& fragmentPath);
    
    GraphicsApplication* pGraphicsApplication = nullptr;
    APImanager* pAPImanager = nullptr;
    Pipeline* pPipeline = nullptr;
    Renderer* pRenderer = nullptr;
    
    std::vector<Object*> pObjects;
    
    bool validationLayers = false;
    uint32_t max_frames_in_flight = NULL;
    bool isObjectPoolInitialized = false;
};

}

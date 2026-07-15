#pragma once

#include "ConsoleText.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

namespace Engine {

class Object {
public:
    Object() = delete;
    Object(APImanager* pManager, Renderer* renderer, const std::vector<Vertex>& vertices);
    
    void draw(Pipeline* pPipeline);
    void cleanUp();
    
    ~Object();
    
private:
    APImanager* pAPImanager = nullptr;
    Renderer* pRenderer = nullptr;
    Mesh* pMesh = nullptr;
};

}

#include "Object.hpp"

namespace Engine {

Object::Object(APImanager* pManager, Renderer* renderer, const std::vector<Vertex>& vertices) :
pRenderer(renderer), pAPImanager(pManager)
{
    pMesh = new Mesh(pAPImanager, vertices);
    pMesh->createVertexBuffer();
}

void Object::draw(Pipeline* pPipeline) {
    pRenderer->drawFrame(pPipeline, pMesh);
}

Object::~Object() {
    delete pMesh;
    
    pAPImanager = nullptr;
    pRenderer = nullptr;
};

//"/Users/user/Projects/GraphicsEngine/Engine/shaders/vert.spv"
//"/Users/user/Projects/GraphicsEngine/Engine/shaders/frag.spv"
//"/../../../GraphicsEngine/Engine/shaders/vert.spv"
//"/../../../GraphicsEngine/Engine/shaders/frag.spv"

}

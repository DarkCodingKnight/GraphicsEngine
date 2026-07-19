#pragma once

#include "ConsoleText.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"

namespace Engine {

class Object {
public:
    Object() = delete;
    Object(APImanager* pManager,
           Renderer* renderer,
           const std::vector<Vertex>& vertices,
           const std::vector<uint16_t>& indices,
           const int max_frames_in_flight,
           const VkDescriptorSetLayout& descriptorLayout);
    
    void draw(Pipeline* pPipeline);
    void cleanUp();
    
    Mesh* getMesh() { return pMesh; };
    
    ~Object();
    
private:
    APImanager* pAPImanager = nullptr;
    Renderer* pRenderer = nullptr;
    Mesh* pMesh = nullptr;
};

}

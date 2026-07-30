#pragma once

#include "ConsoleText.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

namespace Engine {

class Object : public KeyStatusInterface {
public:
    Object() = delete;
    Object(APImanager* pManager,
           Renderer* renderer,
           const std::vector<Vertex>& vertices,
           const std::vector<uint16_t>& indices,
           const int max_frames_in_flight,
           const VkDescriptorSetLayout& descriptorLayout);
    
    void updateKey(std::string key, int scancode, int action, int mods) override {
        std::cout << "Key: " << key << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << "." << std::endl;
    }
    
    void draw(Pipeline* pPipeline);
    void cleanUp();
    
    Mesh* getMesh() { return pMesh; };
    
    ~Object() override;
    
private:
    APImanager* pAPImanager = nullptr;
    Renderer* pRenderer = nullptr;
    Mesh* pMesh = nullptr;
};

}

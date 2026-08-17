#pragma once

#include "stb_image.h"
#include "tiny_obj_loader.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <optional>
#include <glm/glm.hpp>

#include "ConsoleText.hpp"
#include "Window.hpp"

namespace Engine {

struct TextureData {
    stbi_uc* pixelsData;
    int texWidth;
    int texHeight;
    size_t texSize;
    uint32_t mipLevels;
    
    ~TextureData() {
        delete pixelsData;
        pixelsData = nullptr;
    };
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

class ResourceManager : public KeyStatusInterface {
public:
    ResourceManager() {};
    
    static std::optional<std::vector<char>> readFile(const std::string& filename);
    static void loadTexture(const char* texturePath,
                                   TextureData* texData);
    static void loadModel(const char* modelPath,
                          std::vector<Vertex>* pVertices,
                          std::vector<uint32_t>* indices);
    
    void updateKey(std::string key, int scancode, int action, int mods) override {
        std::cout << "Key: " << key
                  << ", scancode: " << scancode
                  << ", action: " << action
                  << ", mods: " << mods
                  << "." << std::endl;
    }
    
    void updateMouse(std::string button, int action, int mods) override {
        std::cout << "Mouse button pressed:" << button << std::endl;
    }
    
    ~ResourceManager() {};
    
private:
    
};

}

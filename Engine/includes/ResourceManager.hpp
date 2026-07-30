#pragma once

#include "stb_image.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <optional>

#include "ConsoleText.hpp"

namespace Engine {

struct TextureData {
    stbi_uc* pixelsData;
    int texWidth;
    int texHeight;
    size_t texSize;
    
    ~TextureData() {
        delete pixelsData;
        pixelsData = nullptr;
    };
};

class ResourceManager {
public:
    ResourceManager() {};
    
    static std::optional<std::vector<char>> readFile(const std::string& filename);
    static TextureData loadTexture(const char* texturePath);
    
    ~ResourceManager() {};
    
private:
    
};

}

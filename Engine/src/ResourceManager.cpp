#include "ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Engine {

std::optional<std::vector<char>> ResourceManager::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if(!file.is_open()) {
        ConsoleText::printError("Failed to open file!", "Resource manager");
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("File was opened!", "Resource manager");
    }
    
    file.seekg(0, std::ios::end); // Moving cursor pos in the end
    size_t fileSize = (size_t)file.tellg(); // Taking pos pointer (taking size)
    
    std::optional<std::vector<char>> buffer(fileSize);
    
    file.seekg(0, std::ios::beg); // Moving cursor pos to the begining
    file.read(buffer.value().data(), fileSize); // Reading the file
    
    file.close();
    
    return buffer;
}

TextureData ResourceManager::loadTexture(const char* texturePath) {
    int texWidth;
    int texHeight;
    int texChannels;
    
    stbi_uc* pixels = stbi_load(texturePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    size_t texSize = texWidth * texHeight * 4;
    
    TextureData texData = { pixels, texWidth, texHeight, texSize };
    
    if (!pixels) {
        ConsoleText::printError("Failed to load texture!", "Resource manager");
        std::exit(0);
    }
    else {
        ConsoleText::printGreen("Texture was loaded!", "Resource manager");
        return texData;
    }
}

}

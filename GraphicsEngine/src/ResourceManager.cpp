#include "ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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

void ResourceManager::loadTexture(const char* texturePath, TextureData* texData) {
    int texWidth;
    int texHeight;
    int texChannels;
    
    stbi_uc* pixels = stbi_load(texturePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    
    size_t texSize = texWidth * texHeight * 4;
    uint32_t mipLevels = std::floor(std::log2(std::max(texWidth, texHeight)));
    
    *texData = { pixels, texWidth, texHeight, texSize, mipLevels };
    
    if (!pixels) {
        ConsoleText::printError("Failed to load texture!", "Resource manager");
        std::exit(0);
    }
    else ConsoleText::printGreen("Texture was loaded!", "Resource manager");
}

void ResourceManager::loadModel(const char* modelPath,
               std::vector<Vertex>* pVertices,
               std::vector<uint32_t>* indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath)) {
        std::string errorMessage = "Failed to load model! " + err;
        ConsoleText::printError(errorMessage, "Resource manager");
        std::exit(0);
    }
    else {
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
                
                vertex.color = { 1.0f, 1.0f, 1.0f };
                
                (*pVertices).push_back(vertex);
                (*indices).push_back(static_cast<uint32_t>((*indices).size()));
            }
        }
        ConsoleText::printGreen("Model was loaded!", "Resource manager");
    }
}

}

#include "ResourceManager.hpp"

namespace Engine {

std::optional<std::vector<char>> ResourceManager::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if(!file.is_open()) {
        ConsoleText::printError("Failed to open file!", "Resource manager");
        return;
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

}

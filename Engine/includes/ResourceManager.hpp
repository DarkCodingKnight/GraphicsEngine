#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <optional>

#include "ConsoleText.hpp"

namespace Engine {

class ResourceManager {
public:
    ResourceManager() {};
    
    std::optional<std::vector<char>> readFile(const std::string& filename);
    
    ~ResourceManager() {};
    
private:
    
};

}

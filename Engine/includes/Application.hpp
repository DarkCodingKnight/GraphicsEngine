#pragma once

#include "Window.hpp"
#include "Vulkan_Init.hpp"

namespace Engine {
    
class Application {
public:
    Application() {};
    
    int Start(int windowWidth,
              int windowHeight,
              const char* windowTitle,
              bool enableValidationLayers);
    void mainLoop();
    
    ~Application() {};
    
private:
    Window* pWindow = nullptr;
    Vulkan* pVulkan = nullptr;
};
    
}

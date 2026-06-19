#pragma once

#include "Window.hpp"
#include "VulkanManager.hpp"

namespace Engine {
    
class Application {
public:
    Application() {};
    
    int Start(int windowWidth,
              int windowHeight,
              const char* windowTitle,
              bool enableValidationLayers);
    void mainLoop();
    void cleanUp();
    
    ~Application() {
        pWindow = nullptr;
        pVulkan = nullptr;
    };
    
private:
    Window* pWindow = nullptr;
    Vulkan* pVulkan = nullptr;
};
    
}

#pragma once

#include "Window.hpp"
#include "ResourceManager.hpp"

namespace Engine {

class GraphicsApplication {
public:
    GraphicsApplication() = delete;
    GraphicsApplication(const int winWidth, const int winHeight, const char* winTitle);
    
    Window* getWindowPtr() { return pWindow; };
    ResourceManager* getResourceManagerPtr() { return pResourceManager; };
    
    void cleanUp();
    
    ~GraphicsApplication() {
        cleanUp();
        
        delete pWindow;
        pWindow = nullptr;
        delete pResourceManager;
        pResourceManager = nullptr;
    };
    
private:
    Window* pWindow = nullptr;
    ResourceManager* pResourceManager = nullptr;
    
    int windowWidth = NULL;
    int windowHeight = NULL;
    
    
};
    
}

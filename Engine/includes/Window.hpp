#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "ConsoleText.hpp"

namespace Engine {

class Window {
public:
    Window() {};
    
    void initWindow(int width, int height, const char* title);
    int isWindowClose();
    void destroyWindow();
    
    GLFWwindow* getWindowPtr() { return pGLFWWindow; };
    
    ~Window() { pGLFWWindow = nullptr; };
    
private:
    GLFWwindow* pGLFWWindow = nullptr;
};

}

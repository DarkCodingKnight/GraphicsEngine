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
    
    ~Window() {};
    
private:
    GLFWwindow* pGLFWWindow = nullptr;
};

}

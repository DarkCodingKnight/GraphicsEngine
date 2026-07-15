#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "ConsoleText.hpp"

namespace Engine {

class WindowInterface {
public:
    WindowInterface() = default;
    
    virtual void updateWindow() = 0;
    
    ~WindowInterface() = default;
};

class Window {
public:
    Window() = delete;
    Window(int width, int height, const char* title);
    
    int isWindowClose();
    void destroyWindow();
    void setUpdateObjects(WindowInterface* updateObject);
    
    GLFWwindow* getGLFWwindowPtr() { return pGLFWWindow; };
    std::vector<WindowInterface*> getUpdateObjects() { return pUpdateObjects; };
    
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        
        for (auto pObject : app->getUpdateObjects()) {
            pObject->updateWindow();
        }
    }
    
    ~Window() { pGLFWWindow = nullptr; };
    
private:
    GLFWwindow* pGLFWWindow = nullptr;
    std::vector<WindowInterface*> pUpdateObjects;
};

}

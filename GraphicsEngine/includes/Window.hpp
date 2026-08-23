#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <unordered_map>

#include "ConsoleText.hpp"

namespace Engine {

class KeyStatusInterface {
public:
    KeyStatusInterface() = default;
    
    virtual void updateKey(int key, int scancode, int action, int mods) = 0;
    virtual void updateMouse(int button, int action, int mods) = 0;
    virtual void updateCursorPosition(double xPos, double yPos) = 0;
    
    virtual ~KeyStatusInterface() = default;
};

class WindowInterface {
public:
    WindowInterface() = default;
    
    virtual void updateWindow() = 0;
    
    virtual ~WindowInterface() = default;
};

class Window {
public:
    Window() = delete;
    Window(int width, int height, const char* title);
    
    bool isWindowClose();
    
    GLFWwindow* getGLFWwindowPtr() { return pGLFWWindow; };
    
    void setFramebufferUpdateSubscribers(WindowInterface* pObject);
    std::vector<WindowInterface*> getFramebufferUpdateObjects() { return pFramebufferUpdateSubscribers; };
    
    static void framebufferResizeCallback(GLFWwindow* pWindow, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pWindow));
        
        for (auto pObject : app->getFramebufferUpdateObjects()) {
            pObject->updateWindow();
        }
    }

    void setKeyCallbackSubscribers(KeyStatusInterface* object);
    std::vector<KeyStatusInterface*> getKeyUpdateSubscribers() { return pKeyUpdateSubscribers; };
    
    static void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pWindow));
        
        for (auto pObject : app->getKeyUpdateSubscribers()) {
            pObject->updateKey(key, scancode, action, mods);
        }
    }
    
    static void mouseCallback(GLFWwindow* pWindow, int button, int action, int mods) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pWindow));
        
        for (auto pObject : app->getKeyUpdateSubscribers()) {
            pObject->updateMouse(button, action, mods);
        }
    }
    
    static void cursorPosCallback(GLFWwindow* pWindow, double xPos, double yPos) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pWindow));
        
        for (auto pObject : app->getKeyUpdateSubscribers()) {
            pObject->updateCursorPosition(xPos, yPos);
        }
    }
    
    ~Window();
    
private:
    GLFWwindow* pGLFWWindow = nullptr;
    std::vector<WindowInterface*> pFramebufferUpdateSubscribers;
    std::vector<KeyStatusInterface*> pKeyUpdateSubscribers;
};

}

#include "Window.hpp"

#include <string>

namespace Engine {

Window::Window(int width, int height, const char* title) {
    if (!glfwInit()) {
        ConsoleText::printError("glfw init error", "Window");
        std::exit(0);
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pGLFWWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!pGLFWWindow) {
        ConsoleText::printError("GLFW window was not created!", "Window");
        glfwTerminate();
        std::exit(0);
    }
    else ConsoleText::printGreen("Window was created!", "Window");
    
    if (glfwVulkanSupported()) {
        ConsoleText::printGreen("GLFW have vulkan support!", "Window");
        
        std::string windowParameters = "Window parameters: WIDTH: "
                                        + std::to_string(width) + ", HEIGHT: "
                                        + std::to_string(height) + ".";
        ConsoleText::printGreen(windowParameters, "Window");
        
        std::string glfwVersion = "GLFW version: " + (std::string)(glfwGetVersionString());
        ConsoleText::printGreen(glfwVersion, "Window");
    }
    else ConsoleText::printError("GLFW have not vulkan support!", "Window");
    
    glfwSetWindowUserPointer(pGLFWWindow, this);
    
    glfwSetFramebufferSizeCallback(pGLFWWindow, framebufferResizeCallback);
}

void Window::setUpdateObjects(WindowInterface* pUpdateObject) {
    pUpdateObjects.push_back(pUpdateObject);
}

int Window::isWindowClose() {
    return glfwWindowShouldClose(pGLFWWindow);
}

void Window::destroyWindow() {
    ConsoleText::printGreen("Window was destroyed!", "Window");
    glfwTerminate();
}

}

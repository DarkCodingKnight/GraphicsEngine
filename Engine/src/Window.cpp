#include "Window.hpp"

#include <string>

namespace Engine {

void Window::initWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        ConsoleText::printError("glfw init error", "Window");
        std::exit(0);
    }
    
    pGLFWWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!pGLFWWindow) {
        ConsoleText::printError("glfwCreateWindow error", "Window");
        glfwTerminate();
        std::exit(0);
    }
    ConsoleText::printGreen("Window was created!", "Window");
    std::string windowParameters = "Window parameters: WIDTH: " + std::to_string(width) + ", HEIGHT: " + std::to_string(height) + ".";
    ConsoleText::printGreen(windowParameters, "Window");
}

int Window::isWindowClose() {
    return glfwWindowShouldClose(pGLFWWindow);
}

void Window::destroyWindow() {
    ConsoleText::printGreen("Window was destroyed", "Window");
    glfwTerminate();
}

}

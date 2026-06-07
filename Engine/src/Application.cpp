#include "Application.hpp"

namespace Engine {

int Application::Start(int windowWidth,
                       int windowHeight,
                       const char* windowTitle,
                       bool enableValidationLayers) {
    ConsoleText::printGreen("Application is started!", "Application");
    
    Window window = Window();
    pWindow = &window;
    pWindow->initWindow(windowWidth, windowHeight, windowTitle);
    
    Vulkan vulkan = Vulkan();
    pVulkan = &vulkan;
    pVulkan->initVulkan(windowTitle, enableValidationLayers);
    
    mainLoop();
}

void Application::mainLoop() {
    while(!(pWindow->isWindowClose())) {
        glfwPollEvents();
    }
    pWindow->destroyWindow();
    pVulkan->cleanUp();
}

}

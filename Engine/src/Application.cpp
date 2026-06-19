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
    
    Vulkan vulkan = Vulkan(enableValidationLayers, pWindow->getWindowPtr());
    pVulkan = &vulkan;
    pVulkan->createApplication(windowTitle);
    
    mainLoop();
    cleanUp();
}

void Application::mainLoop() {
    while(!(pWindow->isWindowClose())) {
        glfwPollEvents();
    }
}

void Application::cleanUp() {
    pVulkan->cleanUp();
    pWindow->destroyWindow();
}

}

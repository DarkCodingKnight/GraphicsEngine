#include "GraphicsApplication.hpp"

namespace Engine {

GraphicsApplication::GraphicsApplication(const int winWidth,
                                         const int winHeight,
                                         const char* winTitle) :
windowWidth(winWidth), windowHeight(winHeight)
{
    ConsoleText::printGreen("Graphics application initialization started!", "Graphics application");
    
    pWindow = new Window(winWidth, winHeight, winTitle);
    pResourceManager = new ResourceManager();
}

void GraphicsApplication::cleanUp() {
    pWindow->destroyWindow();
}

}

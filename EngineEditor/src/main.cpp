//#define RELEASE

#include "iostream"

#include "Application.hpp"
#include "ConsoleText.hpp"

#ifdef RELEASE
bool enableValidationLayers = false;
#else
bool enableValidationLayers = true;
#endif

namespace EngineEditor {
    
class Editor {
public:
    Editor() {};
    
    void StartEngine() {
        application.Start(WINDOW_WIDTH,
                          WINDOW_HEIGHT,
                          TITLE,
                          enableValidationLayers);
    }
    ~Editor() {};
    
private:
    Engine::Application application;
    
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 500;
    const char* TITLE = "GLFW window";
};

}

int main() {
    EngineEditor::Editor editor = EngineEditor::Editor();
    
    try {
        editor.StartEngine();
    } catch (const std::exception& e) {
        ConsoleText::printError("Engine error!", "Engine editor");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

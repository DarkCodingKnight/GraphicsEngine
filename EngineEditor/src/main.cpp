//#define RELEASE
#ifdef RELEASE
bool enableValidationLayers = false;
#else
bool enableValidationLayers = true;
#endif

#include "Application.hpp"

int main(int args, char** argv) {
    EngineEditor::Application app(enableValidationLayers);
    
    try {
        app.start();
        
    } catch (const std::exception& e) {
        std::string message = "Engine error! " + static_cast<std::string>(e.what());
        ConsoleText::printError(message, "Engine editor");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

//const std::vector<Engine::Vertex> vertices = {
//    {{-0.5f,-0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
//    {{ 0.5f,-0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
//    {{ 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }},
//    {{-0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }},
//
//    {{-0.5f,-0.5f,-0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
//    {{ 0.5f,-0.5f,-0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
//    {{ 0.5f, 0.5f,-0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }},
//    {{-0.5f, 0.5f,-0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }}
//};
//
//const std::vector<uint16_t> indices = {
//    0, 1, 2, 2, 3, 0,
//    4, 5, 6, 6, 7, 4
//};

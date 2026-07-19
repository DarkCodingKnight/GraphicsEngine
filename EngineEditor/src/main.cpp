//#define RELEASE

#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>

#include <iostream>

#include "ObjectPool.hpp"
#include "ConsoleText.hpp"

#ifdef RELEASE
bool enableValidationLayers = false;
#else
bool enableValidationLayers = true;
#endif

namespace EngineEditor {
    

class QT_Appilication {
public:
    QT_Appilication() = default;
    
    int start(int argc, char* argv[]) {
        
        // 1. create application
        QApplication app(argc, argv);
        
        // 2. create main window
        static QWidget window;
        window.setWindowTitle("First QT application.");
        window.resize(400, 300);
        
        // 3. create vertical composer
        QVBoxLayout* layout = new QVBoxLayout(&window);
        
        // 4. create text tag
        QLabel* label = new QLabel("START YOUR ENGINEEE!!!!.", &window);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        
        // 5. create button
        QPushButton* button = new QPushButton("Press me to start engine!", &window);
        button->setGeometry(200, 200, 200, 40);
        layout->addWidget(button);
        
        // 6. attach button signal
        QObject::connect(button, &QPushButton::clicked, [label]() {
            window.close();
        });
        
        // 7. show window
        window.show();
        
        // 8. main application loop start
        return app.exec();
    }
    
    ~QT_Appilication() = default;
};

class Editor {
public:
    Editor() {};
    
    void StartEngine() {
        pGraphicsApplication = new Engine::GraphicsApplication(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
        pObjectPool = new Engine::ObjectPool(pGraphicsApplication,
                                             TITLE,
                                             enableValidationLayers,
                                             MAX_FRAMES_IN_FLIGHT,
                                             vertexPath,
                                             fragmentPath);
        
        pObjectPool->createObject(vertices, indices);
        
        mainLoop();
        
        clenup();
    }
    
    void mainLoop() {
        while(pObjectPool->updateObjects()) {};
    }
    
    void clenup() {
        delete pObjectPool;
        delete pGraphicsApplication;
    }
    
    ~Editor() {};
    
private:
    
    Engine::GraphicsApplication* pGraphicsApplication;
    Engine::ObjectPool* pObjectPool;
    
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 500;
    const char* TITLE = "GLFW window";
    
    const std::vector<Engine::Vertex> vertices = {
        {{-0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
        {{ 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
        {{ 0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
        {{-0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }}
    };
    
    const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
    
    const std::string vertexPath = "/Users/user/Projects/GraphicsEngine/Engine/shaders/vert.spv";
    const std::string fragmentPath = "/Users/user/Projects/GraphicsEngine/Engine/shaders/frag.spv";
    
    //"/../../../GraphicsEngine/Engine/shaders/vert.spv"
    //"/../../../GraphicsEngine/Engine/shaders/frag.spv"
};

}

int main(int args, char** argv) {
    EngineEditor::QT_Appilication qt_application = EngineEditor::QT_Appilication();
    
    
    EngineEditor::Editor editor = EngineEditor::Editor();
    
    try {
        qt_application.start(args, argv);
        editor.StartEngine();
    } catch (const std::exception& e) {
        std::string message = "Engine error! " + static_cast<std::string>(e.what());
        ConsoleText::printError(message, "Engine editor");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

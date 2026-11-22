//#define RELEASE
#ifdef RELEASE
bool enableValidationLayers = false;
#else
bool enableValidationLayers = true;
#endif

#include "MotorSimulation.hpp"

int main(int args, char** argv) {
    STM32::MotorSimulation app = STM32::MotorSimulation();
    
    try {
        app.start();
        
    } catch (const std::exception& e) {
        std::string message = "Engine error! " + static_cast<std::string>(e.what());
        ConsoleText::printError(message, "Engine editor");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
#include "Audio.hpp"

namespace Engine {

Audio::Audio() {
    initAudio();
}

void Audio::initAudio() {
    soloud = SoLoud::Soloud();
    wav = SoLoud::Wav();
    
    int result = soloud.init();
    
    if (result != 0) {
        std::string errorMessage = "Failed to intitialize SoLoud, error: " + soloudErrors[result];
        ConsoleText::printError(errorMessage, "Audio");
    }
    else ConsoleText::printGreen("Sound was initialized!", "Audio");
}

void Audio::playSound(const char* sound_name) {
    int result = wav.load(sound_name);
    
    if (result != 0) {
        std::string errorMessage = "Failed to load sound, error: " + soloudErrors[result];
        ConsoleText::printError(errorMessage, "Audio");
    }
    else ConsoleText::printGreen("Sound was loaded!", "Audio");
    
    soloud.play(wav);
}

Audio::~Audio() {
    soloud.deinit();
}

}

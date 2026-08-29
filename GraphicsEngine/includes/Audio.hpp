#pragma once

#include <iostream>
#include <unordered_map>
#include <string>

#include "soloud.h"
#include "soloud_speech.h"
#include "soloud_wav.h"

#include "ConsoleText.hpp"

namespace Engine {

class Audio {
public:
    Audio(const Audio& other) = delete;
    Audio(Audio&& other) = delete;
    Audio& operator=(const Audio& other) = delete;
    Audio& operator=(Audio&& other) = delete;
    Audio();
    
    void playSoundSource(const char* sound_name);
    void setVolume(float volume) { soloud.setGlobalVolume(volume); };
    
    ~Audio();
    
private:
    void initAudio();
    
    SoLoud::Soloud soloud;
    SoLoud::Wav wav;
    SoLoud::Speech speech;
    
    std::unordered_map<int, std::string> soloudErrors = {
        { 0, "SO_NO_ERROR" },
        { 1, "INVALID_PARAMETER" },
        { 2, "FILE_NOT_FOUND" },
        { 3, "FILE_LOAD_FAILED" },
        { 4, "DLL_NOT_FOUND" },
        { 5, "OUT_OF_MEMORY" },
        { 6, "NOT_IMPLEMENTED" },
        { 7, "UNKNOWN_ERROR" }
    };
};

}

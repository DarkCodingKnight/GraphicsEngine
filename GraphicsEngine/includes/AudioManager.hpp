#pragma concept

#include "Audio.hpp"

namespace Engine {

class AudioManager {
public:
    AudioManager();
    AudioManager(const AudioManager& other) = delete;
    AudioManager(AudioManager&& other) = delete;
    AudioManager& operator=(const AudioManager& other) = delete;
    AudioManager& operator=(AudioManager&& other) = delete;

    void play_sound(const char* _sound_name_);
    void play_engine_sound();
    void play_button_sound();
    void play_complete_sound();
    void play_open_sound();

    ~AudioManager();

private:
    Audio* pAudio = nullptr;

    const char* engine_sound_path = "/Users/user/Projects/GameEngine/GraphicsEngine/sounds/UFO_sounds/spaceEngineSmall_001.ogg";
    const char* button_sound_path = "/Users/user/Projects/GameEngine/GraphicsEngine/sounds/Interface_sounds/bong_001.ogg";
    const char* complete_sound_path = "/Users/user/Projects/GameEngine/GraphicsEngine/sounds/Interface_sounds/confirmation_001.ogg";
    const char* open_sound_path = "/Users/user/Projects/GameEngine/GraphicsEngine/sounds/Interface_sounds/question_003.ogg";
};

}
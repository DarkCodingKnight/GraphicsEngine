#include "AudioManager.hpp"

namespace Engine {

AudioManager::AudioManager() {
    pAudio = new Audio();
}

void AudioManager::play_sound(const char* _sound_name_) { pAudio->playSoundSource(_sound_name_); }
void AudioManager::play_engine_sound() { pAudio->playSoundSource(engine_sound_path); }

AudioManager::~AudioManager() {
    delete pAudio;
    pAudio = nullptr;
}

}
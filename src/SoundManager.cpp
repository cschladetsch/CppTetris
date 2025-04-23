#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager() 
    : initialized_(false), muted_(false), volume_(MIX_MAX_VOLUME) {
}

SoundManager::~SoundManager() {
    // Clean up mixer before exit
    if (initialized_) {
        Mix_CloseAudio();
        Mix_Quit();
    }
}

bool SoundManager::initialize() {
    // Force SDL to use PulseAudio
    SDL_setenv("SDL_AUDIODRIVER", "pulseaudio", 1);
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    initialized_ = true;
    
    // Set initial volume
    Mix_Volume(-1, volume_);
    
    return true;
}
bool SoundManager::loadSounds() {
    if (!initialized_) {
        std::cerr << "Cannot load sounds - audio system not initialized" << std::endl;
        return false;
    }
    
    // Load all the sound effects
    bool success = true;
    success &= loadSound(SoundEffect::Move, "resources/sounds/move.wav");
    success &= loadSound(SoundEffect::Rotate, "resources/sounds/rotate.wav");
    success &= loadSound(SoundEffect::Drop, "resources/sounds/drop.wav");
    success &= loadSound(SoundEffect::LineClear, "resources/sounds/clear.wav");
    success &= loadSound(SoundEffect::LevelUp, "resources/sounds/levelup.wav");
    success &= loadSound(SoundEffect::GameOver, "resources/sounds/gameover.wav");
    
    return success;
}

bool SoundManager::loadSound(SoundEffect effect, const std::string& filename) {
    Mix_Chunk* sound = Mix_LoadWAV(filename.c_str());
    
    if (!sound) {
        std::cerr << "Failed to load sound effect: " << filename << " - " 
                  << Mix_GetError() << std::endl;
        return false;
    }
    
    sounds_[effect] = std::unique_ptr<Mix_Chunk, MixChunkDeleter>(sound);
    return true;
}

void SoundManager::playSound(SoundEffect effect) {
    if (!initialized_ || muted_) {
        return;
    }
    
    auto it = sounds_.find(effect);
    if (it != sounds_.end()) {
        Mix_PlayChannel(-1, it->second.get(), 0);
    }
}

void SoundManager::setVolume(int volume) {
    // Convert from 0-100 range to 0-128 range (SDL_mixer volume range)
    volume_ = (volume * MIX_MAX_VOLUME) / 100;
    
    if (volume_ < 0) volume_ = 0;
    if (volume_ > MIX_MAX_VOLUME) volume_ = MIX_MAX_VOLUME;
    
    // Set volume for all channels
    Mix_Volume(-1, volume_);
}

void SoundManager::toggleMute() {
    muted_ = !muted_;
    
    if (muted_) {
        // Store the current volume and set to 0
        Mix_Volume(-1, 0);
    } else {
        // Restore the previous volume
        Mix_Volume(-1, volume_);
    }
}

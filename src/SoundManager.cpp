#include "SoundManager.h"
#include <iostream>
#include <vector>
#include <utility>  // for std::pair

SoundManager::SoundManager() 
    : initialized_(false), muted_(false), volume_(MIX_MAX_VOLUME) {
}

SoundManager::~SoundManager() {
    if (initialized_) {
        Mix_CloseAudio();
        Mix_Quit();
    }
}

bool SoundManager::initialize() {
    // Try multiple audio drivers in order
    const char* drivers[] = {"directsound", "wasapi", "pulseaudio", "alsa", "dummy"};
    bool success = false;
    
    for (const char* driver : drivers) {
        std::cout << "Trying audio driver: " << driver << std::endl;
        SDL_setenv("SDL_AUDIODRIVER", driver, 1);
        
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0) {
            std::cout << "Successfully initialized audio with driver: " << driver << std::endl;
            success = true;
            break;
        }
        std::cerr << "Failed to initialize with " << driver << ": " << Mix_GetError() << std::endl;
    }
    
    if (!success) {
        std::cerr << "SDL_mixer could not initialize with any audio driver! SDL_mixer Error: " << Mix_GetError() << std::endl;
        std::cerr << "Warning: Sound system could not be initialized. Continuing without sound." << std::endl;
        return false;
    }
    
    initialized_ = true;
    Mix_Volume(-1, volume_);
    return true;
}
bool SoundManager::loadSounds() {
    if (!initialized_) {
        std::cerr << "Cannot load sounds - audio system not initialized" << std::endl;
        // Return true to prevent cascading failures
        return true;
    }
    
    bool success = true;
    
    // Check if sound files exist and attempt to load them
    const std::vector<std::pair<SoundEffect, std::string>> soundFiles = {
        {SoundEffect::Move, "resources/sounds/move.wav"},
        {SoundEffect::Rotate, "resources/sounds/rotate.wav"},
        {SoundEffect::Drop, "resources/sounds/drop.wav"},
        {SoundEffect::LineClear, "resources/sounds/clear.wav"},
        {SoundEffect::LevelUp, "resources/sounds/levelup.wav"},
        {SoundEffect::GameOver, "resources/sounds/gameover.wav"}
    };
    
    for (const auto& [effect, filename] : soundFiles) {
        // Check if file exists before attempting to load
        FILE* file = fopen(filename.c_str(), "r");
        if (file) {
            fclose(file);
            success &= loadSound(effect, filename);
        } else {
            std::cerr << "Sound file not found: " << filename << std::endl;
        }
    }
    
    if (!success) {
        std::cerr << "Warning: Some sounds could not be loaded, continuing with partial sound support" << std::endl;
    }
    
    return true; // Always return true to allow game to continue
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
    volume_ = (volume * MIX_MAX_VOLUME) / 100;
    
    if (volume_ < 0) volume_ = 0;
    if (volume_ > MIX_MAX_VOLUME) volume_ = MIX_MAX_VOLUME;
    
    Mix_Volume(-1, volume_);
}

void SoundManager::toggleMute() {
    muted_ = !muted_;
    
    if (muted_) {
        Mix_Volume(-1, 0);
    } else {
        Mix_Volume(-1, volume_);
    }
}

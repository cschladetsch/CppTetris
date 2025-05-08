#pragma once

#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <string>
#include <memory>

// Sound effect types
enum class SoundEffect {
    Move,
    Rotate,
    Drop,
    LineClear,
    LevelUp,
    GameOver
};

class SoundManager {
public:
    SoundManager();
    virtual ~SoundManager();

    // Initialize the audio system
    virtual bool initialize();
    
    // Load sounds from files
    virtual bool loadSounds();
    
    // Play a specific sound effect
    virtual void playSound(SoundEffect effect);
    
    // Set sound volume (0-100)
    virtual void setVolume(int volume);
    
    // Mute/unmute all sounds
    virtual void toggleMute();
    bool isMuted() const { return muted_; }

private:
    // Custom deleter for Mix_Chunk
    struct MixChunkDeleter {
        void operator()(Mix_Chunk* chunk) {
            if (chunk) Mix_FreeChunk(chunk);
        }
    };
    
    // Sound effects container
    std::unordered_map<SoundEffect, std::unique_ptr<Mix_Chunk, MixChunkDeleter>> sounds_;
    
    bool initialized_;
    bool muted_;
    int volume_; // 0-128 (SDL_mixer range)
    
    // Helper method to load a single sound
    bool loadSound(SoundEffect effect, const std::string& filename);
};

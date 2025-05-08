#pragma once

#include "Game.h"
#include "SoundManager.h"

// Mock SoundManager for testing - avoids actual audio initialization
class MockSoundManager : public SoundManager {
public:
    MockSoundManager() : SoundManager() {}
    
    bool initialize() override { return true; }
    bool loadSounds() override { return true; }
    void playSound(SoundEffect) override {}
    void setVolume(int) override {}
    void toggleMute() override {}
};

// Forward declaration
class TetrominoManager;

// Game subclass for testing that doesn't initialize SDL or audio
class TestGame : public Game {
public:
    TestGame() 
        : Game(true) // Pass true to flag this is a test instance
    {
        // Replace sound manager with our mock version
        soundManager_ = std::make_unique<MockSoundManager>();
        
        // Initialize TetrominoManager if it's null (which it will be in test mode)
        if (!tetrominoManager_) {
            tetrominoManager_ = std::make_unique<TetrominoManager>(*this);
        }
    }
};
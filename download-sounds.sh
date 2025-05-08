#!/bin/bash

# Create sounds directory if it doesn't exist
mkdir -p resources/sounds

echo "Creating dummy sound files (since download failed)..."

# Create a silent WAV file for each sound
create_silent_wav() {
    local output_file=$1
    
    # Create a 1 second silent WAV file (44.1kHz, 16-bit, stereo)
    if command -v ffmpeg &> /dev/null; then
        ffmpeg -f lavfi -i "anullsrc=r=44100:cl=stereo" -t 1 -c:a pcm_s16le "$output_file" -y -loglevel error
        echo "Created silent WAV file: $output_file"
    else
        # If ffmpeg is not available, create a minimal WAV header
        echo "RIFF....WAVEfmt ....    ...data...." > "$output_file"
        echo "Created minimal WAV file: $output_file (may not work correctly)"
    fi
}

# Create dummy sound files
create_silent_wav "resources/sounds/move.wav"
create_silent_wav "resources/sounds/rotate.wav"
create_silent_wav "resources/sounds/drop.wav"
create_silent_wav "resources/sounds/clear.wav"
create_silent_wav "resources/sounds/levelup.wav"
create_silent_wav "resources/sounds/gameover.wav"

echo "Dummy sound files created in resources/sounds/"
echo "Note: These are silent files to allow the game to run without crashing."
echo "When you have a proper internet connection or access to the sound files,"
echo "you may want to try downloading real sound effects."

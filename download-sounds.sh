#!/bin/bash

# Create sounds directory if it doesn't exist
mkdir -p resources/sounds

# List of sound URLs from freesound.org (Creative Commons 0 license)
MOVE_SOUND="https://freesound.org/data/previews/573/573580_2282212-lq.mp3"
ROTATE_SOUND="https://freesound.org/data/previews/422/422744_7924764-lq.mp3"
DROP_SOUND="https://freesound.org/data/previews/398/398175_7586547-lq.mp3"
CLEAR_SOUND="https://freesound.org/data/previews/352/352661_6478147-lq.mp3"
LEVELUP_SOUND="https://freesound.org/data/previews/243/243020_4284968-lq.mp3"
GAMEOVER_SOUND="https://freesound.org/data/previews/277/277441_4486188-lq.mp3"

# Download each sound
echo "Downloading sound effects..."

curl -L -o resources/sounds/move.wav $MOVE_SOUND
curl -L -o resources/sounds/rotate.wav $ROTATE_SOUND
curl -L -o resources/sounds/drop.wav $DROP_SOUND
curl -L -o resources/sounds/clear.wav $CLEAR_SOUND
curl -L -o resources/sounds/levelup.wav $LEVELUP_SOUND
curl -L -o resources/sounds/gameover.wav $GAMEOVER_SOUND

echo "Sound effects downloaded to resources/sounds/"
echo "You may need to install ffmpeg and convert mp3 files to wav format if SDL_mixer has trouble loading them:"
echo "For example: ffmpeg -i resources/sounds/move.mp3 resources/sounds/move.wav"

# Make the script executable
chmod +x download-sounds.sh

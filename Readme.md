# C++23 Tetris Game with SDL2

A colorful, feature-rich implementation of the classic Tetris game using modern C++23 and SDL2.

![Tetris Screenshot](screenshot.png)

## Features

- Modern C++23 implementation with smart pointers, concepts, and formatting
- Colorful, 3D-like tetromino blocks with vibrant graphics
- Preview of the next tetromino
- Ghost piece projection showing where the current piece will land
- Score system with bonus points for hard drops
- Level progression that increases game speed
- Line clearing with cascading score multipliers
- Unicode-compatible text rendering for controls display

## Controls

- ** **: Move tetromino left/right
- ****: Rotate tetromino
- ****: Soft drop (move down faster)
- **Space**: Hard drop (instantly drop to the bottom)
- **Enter**: Restart after game over

## Requirements

- C++23 compatible compiler (GCC 13+, Clang 16+, or MSVC 19.33+)
- SDL2 and SDL2_ttf libraries
- CMake 3.14 or higher

## Building from Source

### Linux/macOS

```bash
# Install dependencies (Ubuntu/Debian example)
sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the game
./tetris
```

### Windows (with MSVC)

```cmd
# Clone the repository
git clone https://github.com/cschladetsch/cpp-tetris.git
cd cpp-tetris

# Create build directory
mkdir build
cd build

# Configure and build with CMake
cmake ..
cmake --build . --config Release

# Run the game
Release\tetris.exe
```

## Project Structure

The codebase is organized into modular components:

- `src/Game.cpp`: Core game initialization and main loop
- `src/GameInput.cpp`: Keyboard input and tetromino movement
- `src/GameTetromino.cpp`: Tetromino creation, locking, and line clearing
- `src/GameRenderer.cpp`: Main rendering functions
- `src/GameUI.cpp`: UI elements like sidebar and next piece preview
- `src/GameGhostPiece.cpp`: Ghost piece projection logic
- `src/Tetromino.cpp`: Tetromino behavior and collision detection
- `src/Color.cpp`: Color definitions for tetrominoes
- `include/*.h`: Corresponding header files

## Font Support

The game tries to load a Unicode-compatible font to display arrow symbols for controls. If no suitable font is found, it falls back to a simple text representation. For best visual experience, make sure you have DejaVu Sans or another Unicode font installed.

## License

[MIT License](LICENSE)

## Acknowledgments

- Original Tetris game created by Alexey Pajitnov
- SDL2 developers for the excellent graphics library
- Modern C++ features that make game development more enjoyable

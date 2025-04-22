# C++23 Tetris Game

A modern implementation of the classic Tetris game using C++23 and SDL2.

![Tetris Screenshot](resources/Tetris.gif)

## Features

- Modern C++23 implementation with smart pointers and format strings
- Colorful graphics with 3D-like block effects
- Ghost piece projection showing where the current piece will land
- Preview of the next tetromino
- Increasing difficulty with level progression
- Score multipliers for clearing multiple lines at once
- Smooth controls with wall kicks for rotation

## Controls

- **Left/Right Arrows**: Move tetromino horizontally
- **Up Arrow**: Rotate tetromino
- **Down Arrow**: Soft drop (move down faster)
- **Space**: Hard drop (instantly place at the bottom)
- **Enter**: Restart after game over

## Requirements

- C++23 compatible compiler (GCC 12+, Clang 15+, MSVC 19.32+)
- SDL2 and SDL2\_ttf libraries
- CMake 3.14 or higher

## Building from Source

### Linux/macOS

```bash
# Install dependencies (Ubuntu/Debian example)
sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev

# Clone this repository
git clone https://github.com/cschladetsch/CppTetris.git
cd CppTetris

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
git clone https://github.com/yourusername/CppTetris.git
cd CppTetris

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

The project uses a modular architecture with functionality separated into specialized files:

- `Game.cpp`: Core game initialization and main loop
- `GameInput.cpp`: Handles keyboard input and tetromino movement
- `GameTetromino.cpp`: Manages tetromino creation, locking, and line clearing
- `GameRenderer.cpp`: Core rendering functions
- `GameUI.cpp`: UI elements like sidebar and next piece preview
- `GameGhostPiece.cpp`: Ghost piece projection logic
- `Tetromino.cpp`: Tetromino behavior and rotation
- `Color.cpp`: Color definitions for tetrominoes

## Acknowledgments

- Original Tetris game created by Alexey Pajitnov
- SDL2 developers for the excellent graphics library

## License

MIT


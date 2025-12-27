# Gemini Project: Worms Clone for TI-89

## Project Overview

This project is a 2D turn-based artillery game, a clone of the classic "Worms" franchise. It is written in C and appears to be targeted for the Texas Instruments TI-89 graphing calculator, as indicated by the use of the TIGCC IDE and specific file extensions like `.tpr`.

The game's architecture is based on a state machine that manages the different phases of gameplay (e.g., worm selection, player turn, weapon selection, game over). The C code is structured to mimic object-oriented programming principles through disciplined naming conventions (`ObjectName_methodName`).

Key components of the game include:
*   A state machine (`Game.c`, `Game.h`) to control the game's flow.
*   Game objects like Worms, Weapons, Crates, Mines, and Oil Drums, each with their own logic modules.
*   A physics and collision engine (`PhysCol.c`).
*   A camera system that can pan and follow in-game objects (`Camera.c`).
*   A detailed map and drawing system (`Map.c`, `Draw.c`).
*   A custom key input system (`Keys.h`).

## Building and Running

The project does not use a standard build system like Make. Instead, it relies on a combination of Node.js scripts for asset preparation and the TIGCC IDE for final compilation and linking.

### 1. Asset Generation

The game's sprites are converted from BMP images into C data arrays. This process is handled by a Node.js script.

*   **Command:** `node generate_sprites_c.js`
*   **Action:** This script reads all `.bmp` files from the `SpriteBitmaps/` directory, converts them into C arrays, and generates two files: `c/SpriteData.c` and `h/SpriteData.h`. This must be run whenever sprites are added or modified.

### 2. Concatenating Source (for context)

A script is provided to concatenate all source files into a single text file, likely for analysis or providing context to a large language model. This is not part of the compilation process.

*   **Command:** `node concatenate_project.js`
*   **Action:** Creates a `full_project_context.txt` file containing the contents of all `.c` and `.h` files.

### 3. Compilation (TIGCC IDE)

The final compilation from C source code into a runnable application for the TI-89 is performed within the **TIGCC IDE**.

*   **TODO:** Document the specific steps required within the TIGCC IDE to build the `Worms25.tpr` project file. This would involve opening the project and invoking the build/compile command within the IDE.

### 4. Running the Game

The game is intended to be run on a TI-89 calculator or a compatible emulator.

*   **TODO:** Document the process for transferring the compiled `.tpr` file to a device or emulator and launching it.

## Development Conventions

*   **Pseudo-OOP in C:** The code follows a strict `ObjectName_functionName()` and `ObjectName_variableName` pattern to group related logic and data.
*   **State Machine:** The core game logic is managed by a state machine in `Game.c`. Each game state (e.g., `gameMode_Turn`, `gameMode_WeaponSelect`) is implemented with `_enter`, `_update`, and `_exit` functions. State logic is separated into individual `.c` files and included directly into `Game.c`.
*   **Bitwise Flags:** The project makes extensive use of bitwise operations and bitmasks to efficiently store and query the state of game objects and flags (e.g., `Worm_active`, `Game_stateFlags`).
*   **Header Guards:** All header files use standard `#ifndef`/`#define`/`#endif` include guards.
*   **Globals:** Game state is managed through a set of global variables, which are typically defined in their respective `.c` modules and declared with `extern` in the corresponding `.h` files.

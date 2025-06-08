# INC-Engine

INC-Engine is a 64-bit custom game engine built using SDL2 and OpenGL. It is designed for speed, low-level control, and modularity, following the philosophy of classic id Tech–style engines. The engine is currently in active development and serves as the foundation for the sci-fi first-person shooter game: GION.

This project is intended for developers who want full understanding and ownership of every system, from window initialization to draw calls. INC-Engine avoids unnecessary abstractions and prioritizes clarity, performance, and long-term maintainability.

## Features

- Written in modern C++ (64-bit only)
- Built on SDL2 and OpenGL
- Designed with modular folder structure and tools
- Uses Makefiles or CMake + Ninja
- Dynamically links to core system libraries and dependencies
- Cross-platform structure (initial focus: Windows 10 x64)

## Dependencies (Dynamic Linking)

- SDL2 (window/input)
- OpenAL Soft (audio)
- zlib (compression)
- libjpeg-turbo (image decoding)

All libraries are placed in `W64lib/` and are linked dynamically for minimal executable size and clean separation.

## Folder Structure
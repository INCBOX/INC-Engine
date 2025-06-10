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


Game Architecture & Systems
Entity-Component-System (ECS): If you're not using it yet, ECS can improve flexibility—let you add new object types and behaviors with less coupling.

Scene Graph / Level Streaming: For large planets or solar systems, consider streaming or paginated worlds to keep memory and performance manageable.

Resource Management: Implement caching and reference counting (especially for textures, meshes, sounds) to avoid reloads and leaks.

2. Rendering Enhancements
Lighting & Shadows: Add dynamic sunlight, planet-based ambient occlusion, or shadow mapping for realism.

Atmospheric Effects: Procedural skyboxes, volumetric fog at planet boundaries, planet glows.

Post-processing: Motion blur, bloom, HDR tone mapping—all doable with SDL+OpenGL.

3. Gameplay & Features
Physics & Collisions: Consider an engine like Bullet or simpler OBB/sphere collisions. More realistic landings and gravity assists.

Input Handling: Full joystick/controller support with SDL’s joystick API. Fine control for spaceship navigation.

UI Layer: Use SDL_ttf for instruments, HUD, or a UI system like Dear ImGui for debug menus and in-game dashboards.

4. Data & Serialization
JSON Workflows: Automate import/export of levels, ship configurations, save/load game state.

Tools: Write small editor apps (SDL or ImGui-powered) to visualize JSON data and edit object attributes, scene layout.

Content Pipeline: Implement a build step to compress assets (e.g. ZIP or custom containers) to improve load times.

5. Performance & Profiling
Profiling Tools: Use Tracy, gprof, or SDL’s perf tools to identify bottlenecks in rendering, physics, or I/O.

Multithreading: Decouple physics, asset loading, or AI into worker threads to keep the main loop smooth.

6. Cross-Platform Considerations
Input Abstraction: Wrap SDL input to feed both desktop and mobile inputs if you plan to port later.

Build System: Ensure smooth build on Windows/Linux/macOS (CMake scripts, CI pipeline).

Packaging: Create installers or app bundles with appropriate resource layouts.

7. Developer Experience
Hot Reloading: Watch JSON and script files; reload them during runtime for faster iteration.

Debug Console: Add an in-game console via SDL or ImGui for developer commands (teleport, cheats, spawn actors).

Logging System: Structured logs (info/warning/error) with file output help during debugging.


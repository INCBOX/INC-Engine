# 🔧 INC Engine – Kernel Runtime Launcher
##kernel runtime + modular DLL + hot reload + DOD + ECS


1 Build Kernel Runtime (Launcher)
	- Create minimal launcher.exe
	- Load DLLs dynamically (renderer.dll, shaderapi.dll, game.dll)
	- Implement main loop calling module update/render functions
	- Add hot reload support for DLLs (watch files, unload/reload safely)
	
2 Define Clean Module Interfaces
	- Design simple C++ interface for modules (Init, Update, Render, Shutdown)
	- Pass dependencies (e.g., ShaderAPI to Renderer) during Init
	
3 Create Stub Modules
	- Stub renderer.dll that clears screen or draws simple geometry
	- Stub shaderapi.dll with minimal GPU abstraction
	- Stub game.dll with basic ticking logic
	
4 Implement Hot Reload Logic
	- Monitor DLL changes on disk
	- Safely unload and reload DLLs at runtime
	- Preserve state across reloads if possible
	
5 Expand Game Module with DOD + ECS
	- Organize game data using Data-Oriented Design principles
	- Build Entity-Component-System inside game.dll
	- Process data in cache-friendly batches via systems
	
6 Iterate and Optimize
	- Add multithreading and job system for CPU utilization
	- Add resource streaming for big worlds
	- Build dev tools for debugging, profiling, logging

---

## 🧠 What Is the Kernel Runtime?

The **Kernel Runtime** is the heart of the INC Engine.  
It is a **lean, hot-loadable, zero-dependency** launcher that orchestrates the entire engine by dynamically loading runtime modules.

No monoliths. No bloat. Only power.

---

## 🧩 Core Responsibilities

| Responsibility        | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| 🔌 DLL Loading         | Loads and unloads core engine modules (`renderer.dll`, `shaderapi.dll`, etc.) |
| 🔄 Hot Reloading       | Swaps DLLs at runtime without restarting (`weapons.dll`, `hud.dll`, etc.)     |
| 🧠 Main Loop Control   | Owns and drives the main update/render loop                                 |
| 🧱 Interface Handling  | Verifies and routes `CreateInterface()` across modules                      |
| 📜 State Routing       | Manages bootstraps, scene transitions, shutdown order                      |

---

## 🔥 Example Load Chain

INC_Launcher.exe
├── Loads: renderer.dll
│ └── Loads: shaderapi.dll
│ └── Loads: shaderapi_gl.dll / shaderapi_dx.dll / shaderapi_vk.dll
├── Loads: input.dll
├── Loads: movementphysics.dll
└── Dynamically: load weapons.dll, tps.dll, editor.dll as needed


---

## ⚙️ Key Concepts

### 🔧 Kernel Runtime
- Executable: `INC_Launcher.exe`
- Responsibilities:
  - DLL load/unload
  - Main game loop control
  - Dev console command processing
  - Interface registry management

Think of it as your **engine kernel** — minimal but absolute control.

---

### 📦 Modules (DLLs)
Each system is a standalone DLL:
- `renderer.dll`, `shaderapi.dll`, `fps.dll`, `editor.dll`, etc.
- Every module must export:
```cpp
extern "C" __declspec(dllexport)
void* CreateInterface(const char* version);

All data passed through strict, versioned interfaces (IRenderer, IShaderAPI, etc.)

🧠 Interface Layer
Ensures compatibility

Enforces API boundaries


Enables hot-replacement of subsystems

🧨 Runtime Commands (Console)
> load weapons
> unload editor
> reload movementphysics
> set_renderer shaderapi_vk

💥 DOD & ECS Compatibility
✅ Data-Oriented Design (DOD): Efficient memory access & parallelism

✅ ECS-ready: Systems are swappable, scalable, async-friendly

⚠️ You write systems as DLLs, not just classes



🛡️ Future-Proofing
Interface versioning

Live module swapping

Layered debug hooks

Independent memory allocators per DLL (optional)

No shared static globals



✅ Design Checklist
 Kernel runtime control loop

 Dynamic DLL loading/unloading

 Interface-based module system

 Hot reload manager (watch/reload DLLs live)

 Scene-based system loading

 Dev console input + runtime reload

 ECS/DOD integration layer
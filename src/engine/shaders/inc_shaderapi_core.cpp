// INC_SHADERAPI_CORE
// Core ShaderAPI implementation and global instance management.
// Responsible for initializing and shutting down the rendering backend.
//
// Currently, the OpenGL backend is hardcoded for quick testing.
// Future versions should support runtime/backend selection based on config/platform.
//
// Includes support for:
// - OpenGL backend (ShaderAPIGL)
// - DirectX backend (ShaderAPIDX) [stub]
// - Vulkan backend (ShaderAPIVulkan) [stub]
// - DDS texture loading handled by the respective backend implementation.
//
// This file manages the global ShaderAPI instance accessed via INC::g_pShaderAPI.
//

#include "shaderapi.h"
#include <iostream>

// Backend implementation includes:
// For now, OpenGL backend is included directly.
// You can switch these to conditional compilation or dynamic backend selection as needed.
#include "inc_shaderapi_gl.cpp"     // OpenGL backend implementation
//#include "inc_shaderapi_dx.cpp"   // DirectX backend implementation (TODO)
//#include "inc_shaderapi_vk.cpp"   // Vulkan backend implementation (TODO)

namespace INC {

// Global ShaderAPI pointer accessible throughout the engine
IShaderAPI* g_pShaderAPI = nullptr;

// Initialize the ShaderAPI system.
// This function selects and creates an instance of the rendering backend.
// Currently hardcoded to OpenGL.
// TODO: Add config-based or platform-based backend selection.
void InitShaderAPI() {
    if (g_pShaderAPI) {
        std::cerr << "[ShaderAPI] Warning: ShaderAPI already initialized!\n";
        return;
    }

    // Instantiate the OpenGL backend implementation.
    g_pShaderAPI = new ShaderAPIGL();

    std::cout << "[ShaderAPI] Initialized with OpenGL backend\n";
}

// Shutdown the ShaderAPI system.
// Deletes the global ShaderAPI instance and cleans up resources.
void ShutdownShaderAPI() {
    if (!g_pShaderAPI) {
        std::cerr << "[ShaderAPI] Warning: ShaderAPI not initialized or already shutdown.\n";
        return;
    }

    delete g_pShaderAPI;
    g_pShaderAPI = nullptr;

    std::cout << "[ShaderAPI] Shutdown completed\n";
}

} // namespace INC

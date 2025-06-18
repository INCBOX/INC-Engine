#include "shaderapi/shaderapi.h"
#include "shaderapi/shaderapi_gl.h"  // Current backend: OpenGL

// Global pointer to the active ShaderAPI (accessible everywhere)
ShaderAPICore* g_pShaderAPI = nullptr;

// Factory function to create the backend
ShaderAPICore* CreateShaderAPI()
{
    if (!g_pShaderAPI)
    {
        // Choose backend — currently OpenGL only
        g_pShaderAPI = new ShaderAPI_GL();
    }
    return g_pShaderAPI;
}

// Clean up the current ShaderAPI instance
void DestroyShaderAPI()
{
    delete g_pShaderAPI;
    g_pShaderAPI = nullptr;
}

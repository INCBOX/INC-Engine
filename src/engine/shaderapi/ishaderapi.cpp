
#include "shaderapi/shaderapi.h"
#include "shaderapi/shaderapi_gl.h"

static ShaderAPICore* g_pShaderAPI = nullptr;

ShaderAPICore* CreateShaderAPI()
{
    if (!g_pShaderAPI)
        g_pShaderAPI = new ShaderAPI_GL();
    return g_pShaderAPI;
}

void DestroyShaderAPI()
{
    delete g_pShaderAPI;
    g_pShaderAPI = nullptr;
}

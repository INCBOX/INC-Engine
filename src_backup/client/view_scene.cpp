// /src/client/view_scene.cpp

#include "shaders/irendersystem.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"

static float g_TestTriangle[] = {
    -0.5f, -0.5f, 0.f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, 0.f,  0.f, 1.f, 0.f,
     0.0f,  0.5f, 0.f,  0.f, 0.f, 1.f
};

void Render_LoadMap(const nlohmann::json& mapData) {
    // Future: spawn entities, upload map geometry
}

void Render_DrawScene(IRenderSystem* render, int width, int height) {
    render->BeginFrame();

    // This will draw the test triangle temporarily
    render->DrawTriangle(g_TestTriangle, sizeof(g_TestTriangle));

    render->EndFrame();
}

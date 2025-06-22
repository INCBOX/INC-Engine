// Entrypoint header: Public-facing API — exposes CreateGPUAPI() and g_pGPURender

#pragma once

#include "shaderapi/gpu_render_interface.h"

extern IGPURenderInterface* g_pGPURender;

IGPURenderInterface* CreateGPUAPI();
void DestroyGPUAPI();

#pragma once
#include "mathlib/camera_f.h"
#include "mathlib/camera_d.h"
#include "camera_manager.h"
#include "shaderapi/gpu_render_interface.h"

Camera_f& GetMainCamera_f();
Camera_d& GetMainCamera_d();

extern CameraManager g_CameraManager;

CameraManager& GetCameraManager();


// Definition of global GPU render interface pointer
// extern IGPURenderInterface* g_pGPURender;  <-- THIS SHIT NOT GOOD..

// Expose the currently loaded GPU rendering interface to other engine systems
IGPURenderInterface* GetRenderInterface();
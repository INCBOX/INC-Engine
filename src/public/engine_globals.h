#pragma once
#include "mathlib/camera_f.h"
#include "mathlib/camera_d.h"
#include "camera_manager.h"

Camera_f& GetMainCamera_f();
Camera_d& GetMainCamera_d();

extern CameraManager g_CameraManager;

CameraManager& GetCameraManager();

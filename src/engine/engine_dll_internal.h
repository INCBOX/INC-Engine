// engine_internal.h 	Wrapper/shim for non-exported internal usage (if needed)

#pragma once

bool Engine_Internal_Init(void* nativeWindowHandle);
bool Engine_Internal_RunFrame(float deltaTime);
void Engine_Internal_Shutdown();
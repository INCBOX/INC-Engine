#pragma once
#include <cstdarg>

void EngineLog_Init(const char* filename = "logs/engine.log");
void EngineLog_Shutdown();
void EngineLog(const char* fmt, ...);
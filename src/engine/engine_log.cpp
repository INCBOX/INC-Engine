#include "engine_log.h"
#include <cstdio>
#include <filesystem>
#include <mutex>

static FILE* s_LogFile = nullptr;
static std::mutex s_LogMutex;

void EngineLog_Init(const char* filename) {
    std::filesystem::create_directories("logs");
    s_LogFile = fopen(filename, "w");
}

void EngineLog_Shutdown() {
    if (s_LogFile) {
        fclose(s_LogFile);
        s_LogFile = nullptr;
    }
}

void EngineLog(const char* fmt, ...) {
    if (!s_LogFile) return;
    std::lock_guard<std::mutex> lock(s_LogMutex);

    va_list args;
    va_start(args, fmt);
    vfprintf(s_LogFile, fmt, args);
    fprintf(s_LogFile, "\n");
    fflush(s_LogFile);
    va_end(args);
}
// src/engine/sys_logging.cpp

#include "engine/ilogging.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>

static FILE* logFile = nullptr;

static void OpenLog()
{
    if (!logFile)
        logFile = fopen("engine.log", "w");
}

static void PrintTime()
{
    time_t now = time(nullptr);
    tm* ltime = localtime(&now);
    fprintf(logFile, "[%02d:%02d:%02d] ", ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
}

void Msg(const char* fmt, ...)
{
    OpenLog();
    PrintTime();
    fprintf(logFile, "[Msg] ");

    va_list args;
    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile);

    // Console echo
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void Warning(const char* fmt, ...)
{
    OpenLog();
    PrintTime();
    fprintf(logFile, "[Warning] ");

    va_list args;
    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile);

    // Console echo (maybe in yellow if you support it)
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void Error(const char* fmt, ...)
{
    OpenLog();
    PrintTime();
    fprintf(logFile, "[Error] ");

    va_list args;
    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile);

    // Console and hard failure
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    exit(1);  // Or trigger an assert/crash
}
// dll_exports.h
#pragma once

#if defined(_WIN32)
  #define DLL_EXPORT __declspec(dllexport)
  #define DLL_IMPORT __declspec(dllimport)  // optional, for future use
#else
  #define DLL_EXPORT __attribute__((visibility("default")))
  #define DLL_IMPORT  // no import needed on Unix
#endif

// dll_loader.h
#pragma once
#include "dll_platform.h"

inline LibHandle LoadLib(const char* path) {
#if defined(_WIN32)
  return LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
  return dlopen(path, RTLD_NOW);
#endif
}

inline void* GetLibProc(LibHandle lib, const char* name) {
#if defined(_WIN32)
  return reinterpret_cast<void*>(GetProcAddress(lib, name));
#else
  return dlsym(lib, name);
#endif
}

inline void CloseLib(LibHandle lib) {
#if defined(_WIN32)
  FreeLibrary(lib);
#else
  if (lib) dlclose(lib);
#endif
}
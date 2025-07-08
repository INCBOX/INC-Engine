// COMBINE WITH FILESYSTEM PUBLIC HEADERS

#pragma once

#if defined(_WIN32)
  #include <Windows.h>
  using LibHandle = HMODULE;

  #define DLL_EXPORT __declspec(dllexport)

  #define LoadLib(path) LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
  #define GetLibProc(lib, name) GetProcAddress(lib, name)
  #define CloseLib(lib) FreeLibrary(lib)

#else
  #include <dlfcn.h>
  using LibHandle = void*;

  #define DLL_EXPORT __attribute__((visibility("default")))

  #define LoadLib(path) dlopen(path, RTLD_NOW)
  #define GetLibProc(lib, name) dlsym(lib, name)
  #define CloseLib(lib) if (lib) dlclose(lib)

#endif

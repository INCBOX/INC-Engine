// dll_platform.h
#pragma once

#if defined(_WIN32)
  #include <Windows.h>
  using LibHandle = HMODULE;
#else
  #include <dlfcn.h>
  using LibHandle = void*;
#endif
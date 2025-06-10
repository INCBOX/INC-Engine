# To enable parallel builds, run: make -jN (e.g., make -j8)

# ======================================================
# INC-Engine Makefile
# Builds the game engine using MinGW-w64 on Windows
# ======================================================

# -------------------------------
# Paths to SDL2 and GLAD
# -------------------------------
SDL2_DIR = W64lib/sdl2
GLAD_DIR = W64lib/glad

SDL2_INCLUDE = $(SDL2_DIR)/include
SDL2_LIB     = $(SDL2_DIR)/lib
SDL2_DLL     = $(SDL2_DIR)/bin/SDL2.dll

GLAD_INCLUDE = $(GLAD_DIR)/include
GLAD_SRC     = $(GLAD_DIR)/src/glad.c

# -------------------------------
# Compiler and Flags
# -------------------------------
CC = g++

# Compiler flags:
# - C++17
# - 64-bit
# - Automatically include all subdirectories of engine/
# - Include SDL2, GLAD, stb_image, and nlohmann

CFLAGS = -std=c++17 -O2 -m64 \
	$(addprefix -I, $(shell find engine -type d)) \
	-I$(SDL2_INCLUDE) \
	-I$(GLAD_INCLUDE) \
	-IW64lib/stb \
	-IW64lib/nlohmann/include

# Linker flags:
# - Console subsystem
# - SDL2, OpenGL, and optionally -lmingw32
# ⚠️ lmingw32 is only needed if you are not using SDL_main correctly (keep for now)
LDFLAGS = -mconsole -lmingw32 -L$(SDL2_LIB) -lSDL2main -lSDL2 -lopengl32

# -------------------------------
# Source Files
# -------------------------------
# Updated: Engine.cpp moved to root
SRC = engine/engine.cpp \
      $(wildcard engine/HUD/*.cpp) \
      $(wildcard engine/Console/*.cpp) \
      $(wildcard engine/Fonts/*.cpp) \
      $(wildcard engine/Camera/*.cpp) \
      $(wildcard engine/Levels/*.cpp) \
      $(wildcard engine/Skybox/*.cpp) \
      $(GLAD_SRC)

# -------------------------------
# Output Executable
# -------------------------------
OUT = bin/INC-Engine.exe

# -------------------------------
# Default Build Target
# -------------------------------
all: $(OUT) postbuild

# -------------------------------
# Executable Build Rule
# -------------------------------
$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

# -------------------------------
# Post-Build: Copy DLLs and Run
# -------------------------------
postbuild:
	@echo Checking and copying DLLs if needed...
	@if [ ! -f bin/SDL2.dll ]; then cp $(SDL2_DLL) bin/SDL2.dll; fi
	@if [ ! -f bin/libstdc++-6.dll ]; then cp W64lib/stb/bin/libstdc++-6.dll bin/; fi
	@if [ ! -f bin/libgcc_s_seh-1.dll ]; then cp W64lib/stb/bin/libgcc_s_seh-1.dll bin/; fi
	@if [ ! -f bin/libwinpthread-1.dll ]; then cp W64lib/stb/bin/libwinpthread-1.dll bin/; fi
	@echo Build complete.

# -------------------------------
# Clean Target
# -------------------------------
clean:
	rm -f bin/*.exe
.PHONY: all clean

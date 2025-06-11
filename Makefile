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
CC = gcc
CXX = g++

# Compiler flags:
# - C++17 for C++ files, C11 for C files
# - 64-bit
# - Automatically include all subdirectories of engine/
# - Include SDL2, GLAD, stb_image, and nlohmann

CFLAGS = -std=c11 -O2 -m64 \
	$(addprefix -I, $(shell find engine -type d)) \
	-I$(SDL2_INCLUDE) \
	-I$(GLAD_INCLUDE) \
	-IW64lib/stb \
	-IW64lib/nlohmann/include

CXXFLAGS = -std=c++17 -O2 -m64 \
	$(addprefix -I, $(shell find engine -type d)) \
	-I$(SDL2_INCLUDE) \
	-I$(GLAD_INCLUDE) \
	-IW64lib/stb \
	-IW64lib/nlohmann/include

# Linker flags:
# - Console subsystem
# - Statically link only MinGW runtime libs
# - Dynamically link SDL2 and Windows system libs
# - lmingw32 is only needed if you are not using SDL_main correctly (keep for now)
LDFLAGS = -mconsole -lmingw32 -L$(SDL2_LIB) -lSDL2main -lSDL2 -lopengl32 \
          -static-libgcc -static-libstdc++ /mingw64/lib/libwinpthread.a
	
# -------------------------------
# Source Files
# -------------------------------
SRC_CPP = engine/engine_CSGO.cpp \
      engine/engine_Globals.cpp \
      engine/engine_Render.cpp \
      engine/engine_Render_Debug.cpp \
      engine/engine_Shaders.cpp \
      engine/engine_STB_Wrapper.cpp \
      $(wildcard engine/mathlib/*.cpp) \
      $(wildcard engine/camera/*.cpp) \
      $(wildcard engine/levels/*.cpp)

SRC_C = $(GLAD_SRC)
	  
# -------------------------------
# BUILDING .O FILES
# -------------------------------
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_C = $(SRC_C:.c=.o)
OBJ = $(OBJ_CPP) $(OBJ_C)

# -------------------------------
# Output Executable
# -------------------------------
OUT = bin/INC-Engine.exe

# -------------------------------
# Default Build Target
# -------------------------------
all: $(OUT)

# -------------------------------
# Executable Build Rule
# -------------------------------
$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT) $(LDFLAGS)


# -------------------------------
# Compile Rule for C++ files
# -------------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------------
# Compile Rule for C files
# -------------------------------
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# -------------------------------
# Post-Build: Copy DLLs and Run
# -------------------------------
# postbuild:
# 	@echo Checking and copying DLLs if needed...
# 	@if [ ! -f bin/SDL2.dll ]; then cp $(SDL2_DLL) bin/SDL2.dll; fi
# 	@if [ ! -f bin/libstdc++-6.dll ]; then cp W64lib/stb/bin/libstdc++-6.dll bin/; fi
# 	@if [ ! -f bin/libgcc_s_seh-1.dll ]; then cp W64lib/stb/bin/libgcc_s_seh-1.dll bin/; fi
# 	@if [ ! -f bin/libwinpthread-1.dll ]; then cp W64lib/stb/bin/libwinpthread-1.dll bin/; fi
# 	@echo Build complete.

# -------------------------------
# Clean Target
# -------------------------------
clean:
	find . -name "*.o" -delete
	rm -f bin/*.exe

.PHONY: all clean
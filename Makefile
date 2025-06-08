# Paths
SDL2_DIR = W64lib/sdl2
GLAD_DIR = W64lib/glad

SDL2_INCLUDE = $(SDL2_DIR)/include
SDL2_LIB = $(SDL2_DIR)/lib
SDL2_DLL = $(SDL2_DIR)/bin/SDL2.dll

GLAD_INCLUDE = $(GLAD_DIR)/include
GLAD_SRC = $(GLAD_DIR)/src/glad.c

# Compiler
CC = g++
CFLAGS = -std=c++17 -m64 -Iengine/include -I$(SDL2_INCLUDE) -I$(GLAD_INCLUDE)
LDFLAGS = -mconsole -L$(SDL2_LIB) -lSDL2 -lSDL2main -lopengl32

# Sources
SRC = $(wildcard engine/src/*.cpp) $(GLAD_SRC)
OUT = bin/INC-Engine.exe

all: $(OUT)
	@echo "Copying SDL2.dll to bin/"
	cp $(SDL2_DLL) bin/

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f bin/*.exe

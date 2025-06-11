#include <SDL2/SDL.h>
#include <SDL2/SDL_messagebox.h>
#include <iostream>
#include <fstream>
#include <direct.h>  // for _getcwd on Windows

int main(int argc, char* argv[]) {
    char cwd[1024];
    _getcwd(cwd, sizeof(cwd));

    std::string msg = std::string("Current Working Directory:\n") + cwd;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "CWD", msg.c_str(), nullptr);

    std::ofstream log("log.txt");
    if (!log) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Log Write Failed", "Could not create log.txt", nullptr);
        return 1;
    }

    log << "Logging started\n";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Log", "log.txt written successfully", nullptr);
    return 0;
}
#include <Windows.h>
#include <iostream>
#include <string>
#include <filesystem>

typedef void (__stdcall *EngineRunFn)();
typedef bool (*FSInitFn)(const std::string&);

int main(int argc, char** argv) {
    // Get path to launcher.exe
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = std::filesystem::path(exePath).parent_path().string();

    // Step 1: Check for -game <modname> argument
    std::string gameDir;
    for (int i = 0; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "-game") {
            gameDir = exeDir + "\\" + argv[i + 1];
            break;
        }
    }

    // Step 2: Auto-detect game folder if -game not passed
    if (gameDir.empty()) {
        for (const auto& entry : std::filesystem::directory_iterator(exeDir)) {
            if (entry.is_directory()) {
                std::string candidate = entry.path().string() + "\\gameinfo.txt";
                if (std::filesystem::exists(candidate)) {
                    gameDir = entry.path().string();
                    break;
                }
            }
        }
    }

    // Step 3: Final gameinfo path check
    std::string gameinfoPath = gameDir + "\\gameinfo.txt";
    if (!std::filesystem::exists(gameinfoPath)) {
        MessageBoxA(NULL, "Could not find gameinfo.txt", "Fatal Error", MB_ICONERROR);
        return -1;
    }

    std::cout << "[Launcher] Using gameinfo.txt: " << gameinfoPath << "\n";

    // Step 4: Load filesystem
    HMODULE fsDLL = LoadLibraryExA("bin\\filesystem_stdio.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!fsDLL) {
        MessageBoxA(NULL, "Failed to load filesystem_stdio.dll", "Error", MB_ICONERROR);
        return -2;
    }

    FSInitFn FS_Init = (FSInitFn)GetProcAddress(fsDLL, "FS_Init");
    if (!FS_Init || !FS_Init(gameinfoPath)) {
        MessageBoxA(NULL, "Filesystem failed to initialize", "Error", MB_ICONERROR);
        return -3;
    }

    // Step 5: Load engine
    HMODULE engineDLL = LoadLibraryExA("bin\\engine.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!engineDLL) {
        MessageBoxA(NULL, "Failed to load engine.dll", "Error", MB_ICONERROR);
        return -10;
    }

    EngineRunFn Engine_Run = (EngineRunFn)GetProcAddress(engineDLL, "Engine_Run");
    if (!Engine_Run) {
        MessageBoxA(NULL, "Missing Engine_Run() in engine.dll", "Fatal Error", MB_ICONERROR);
        return -11;
    }

    // Step 6: Run engine
    Engine_Run();
    return 0;
}
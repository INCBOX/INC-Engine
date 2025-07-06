#pragma once
#include <string>

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual std::string ResolvePath(const std::string& relativePath) = 0;
};

// This is the exported getter used by engine or other DLLs to retrieve the interface.
extern "C" __declspec(dllexport) IFileSystem* GetFileSystemInterface();
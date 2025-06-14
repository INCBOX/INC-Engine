#pragma once
#include <string>
#include <cstdint>

//-----------------------------------------------------------------------------
// Forward declarations for geometry/model data structures
// Represent model geometry (vertices, indices, bones, etc.)
// Used as input/output for GPU upload in ShaderAPI implementations.
//-----------------------------------------------------------------------------
struct IMDL_CPU; // CPU-side raw geometry
struct IMDL_GPU; // GPU-side buffers

//-----------------------------------------------------------------------------
// IShaderAPI - Abstract interface for shader and rendering backend implementations.
// Enables pluggable backends: OpenGL, DirectX, Vulkan, etc.
// All rendering and resource management calls go through this interface.
//
// Notes:
// - DDS texture loading currently supports basic 2D textures only.
// - Mipmap support (e.g. via glCompressedTexImage2D) is planned but not implemented.
// - Future support includes cubemaps, 3D DDS textures, and GPU resource lifetime management.
//-----------------------------------------------------------------------------
class IShaderAPI {
public:
    virtual ~IShaderAPI() = default;

    // Loads vertex and fragment shaders from relative file paths,
    // compiles and links them into a GPU shader program.
    // Returns an internal program ID handle.
    virtual uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) = 0;

    // Sets the active shader program for rendering.
    virtual void UseShader(uint32_t programID) = 0;

    // Sets a float uniform by name for the given shader program.
    virtual void SetUniform1f(uint32_t program, const std::string& name, float value) = 0;

    // Sets an int uniform by name for the given shader program.
    virtual void SetUniform1i(uint32_t program, const std::string& name, int value) = 0;

    // Uploads geometry data (vertices, indices) from IMDL_CPU to GPU buffers,
    // storing GPU handles inside the IMDL_GPU instance.
    virtual void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& in) = 0;

    // Issues draw calls to render the given IMDL_GPU.
    virtual void DrawModel(const IMDL_GPU& model) = 0;

    // Loads a DDS texture from raw data in memory.
    // - data: pointer to DDS bytes in memory
    // - size: size in bytes of DDS data
    // - width, height: texture dimensions
    // - ddsFormat: compressed pixel format (e.g., DXT1, DXT5)
    // Returns a texture handle for GPU resource.
    virtual uint32_t LoadDDSTexture(const void* data, size_t size, int width, int height, uint32_t ddsFormat) = 0;

    // Binds a loaded texture to the specified texture unit (default 0).
    virtual void BindTexture(uint32_t textureID, int slot = 0) = 0;
};

//-----------------------------------------------------------------------------
// Concrete backend implementations of IShaderAPI for graphics APIs.
//-----------------------------------------------------------------------------

// OpenGL implementation of ShaderAPI.
// Uses OpenGL calls to implement all methods.
// TODO: Add mipmap generation, cubemap support, and GPU resource lifetime management.
class ShaderAPIGL : public IShaderAPI {
public:
    virtual ~ShaderAPIGL() override = default;

    uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) override;
    void UseShader(uint32_t programID) override;
    void SetUniform1f(uint32_t program, const std::string& name, float value) override;
    void SetUniform1i(uint32_t program, const std::string& name, int value) override;

    void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& in) override;
    void DrawModel(const IMDL_GPU& model) override;

    uint32_t LoadDDSTexture(const void* data, size_t size, int width, int height, uint32_t ddsFormat) override;
    void BindTexture(uint32_t textureID, int slot = 0) override;
};

// DirectX implementation of ShaderAPI (stubbed).
// TODO: Implement full DirectX backend.
class ShaderAPIDX : public IShaderAPI {
public:
    virtual ~ShaderAPIDX() override = default;

    uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) override;
    void UseShader(uint32_t programID) override;
    void SetUniform1f(uint32_t program, const std::string& name, float value) override;
    void SetUniform1i(uint32_t program, const std::string& name, int value) override;

    void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& in) override;
    void DrawModel(const IMDL_GPU& model) override;

    uint32_t LoadDDSTexture(const void* data, size_t size, int width, int height, uint32_t ddsFormat) override;
    void BindTexture(uint32_t textureID, int slot = 0) override;
};

// Vulkan implementation of ShaderAPI (stubbed).
// TODO: Implement full Vulkan backend.
class ShaderAPIVulkan : public IShaderAPI {
public:
    virtual ~ShaderAPIVulkan() override = default;

    uint32_t LoadShader(const std::string& vertexRelPath, const std::string& fragRelPath) override;
    void UseShader(uint32_t programID) override;
    void SetUniform1f(uint32_t program, const std::string& name, float value) override;
    void SetUniform1i(uint32_t program, const std::string& name, int value) override;

    void UploadGeometry(IMDL_GPU& out, const IMDL_CPU& in) override;
    void DrawModel(const IMDL_GPU& model) override;

    uint32_t LoadDDSTexture(const void* data, size_t size, int width, int height, uint32_t ddsFormat) override;
    void BindTexture(uint32_t textureID, int slot = 0) override;
};

//-----------------------------------------------------------------------------
// Global ShaderAPI instance accessible across engine modules.
// Initialization code creates and manages the backend instance
// based on platform/configuration.
//
// Usage example:
//    INC::g_pShaderAPI->LoadShader(...);
//
//-----------------------------------------------------------------------------
namespace INC {
    extern IShaderAPI* g_pShaderAPI;

    // Initializes the global ShaderAPI backend instance.
    void InitShaderAPI();

    // Cleans up and destroys the global ShaderAPI backend instance.
    void ShutdownShaderAPI();
}
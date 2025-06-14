#pragma once
#include <string>
#include <cstdint>

// ---------------------------------------------------
// Forward declarations and includes for backend types

#ifdef _WIN32
    #include <d3d11.h>
    #include <wrl/client.h>
#endif

#ifdef USE_VULKAN
    #include <vulkan/vulkan.h>
#endif

// ---------------------------------------------------
// Represents a GPU-ready model instance with resource handles and metadata

struct IMDL_GPU {
    uint32_t indexCount = 0;         // Number of indices to draw
    std::string materialName;        // Material/texture name used by this model

    // ---------------------------------------------------
    // OpenGL backend handles
    uint32_t vao = 0;                // Vertex Array Object
    uint32_t vbo = 0;                // Vertex Buffer Object
    uint32_t ebo = 0;                // Element Buffer Object (index buffer)

    // ---------------------------------------------------
    // DirectX 11 backend handles
#ifdef _WIN32
    Microsoft::WRL::ComPtr<ID3D11Buffer> dxVertexBuffer;   // Vertex buffer
    Microsoft::WRL::ComPtr<ID3D11Buffer> dxIndexBuffer;    // Index buffer
#endif

    // ---------------------------------------------------
    // Vulkan backend handles
#ifdef USE_VULKAN
    VkBuffer vkVertexBuffer = VK_NULL_HANDLE;              // Vulkan vertex buffer
    VkBuffer vkIndexBuffer  = VK_NULL_HANDLE;              // Vulkan index buffer
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;        // Bound descriptor set
#endif

    // ---------------------------------------------------
    // Extend this struct as needed for backend-specific state
};
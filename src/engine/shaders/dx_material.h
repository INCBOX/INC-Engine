#pragma once
#include <string>
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>          // For Microsoft::WRL::ComPtr

class MaterialDX {
public:
    MaterialDX() = default;
    ~MaterialDX() = default;

    // Initialize shaders from compiled shader bytecode files
    bool LoadShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, ID3D11Device* device);

    // Set texture resource view for the material
    void SetTexture(ID3D11ShaderResourceView* textureSRV);

    // Bind shaders and texture for rendering
    void Bind(ID3D11DeviceContext* context);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

    bool LoadShaderBytecode(const std::string& filePath, std::vector<char>& outBytecode);
};

#include "dx_material.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <d3dcompiler.h> // For shader reflection, optional

bool MaterialDX::LoadShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, ID3D11Device* device) {
    std::vector<char> vsBytecode;
    if (!LoadShaderBytecode(vertexShaderPath, vsBytecode)) {
        std::cerr << "[MaterialDX] Failed to load vertex shader bytecode: " << vertexShaderPath << "\n";
        return false;
    }

    std::vector<char> psBytecode;
    if (!LoadShaderBytecode(pixelShaderPath, psBytecode)) {
        std::cerr << "[MaterialDX] Failed to load pixel shader bytecode: " << pixelShaderPath << "\n";
        return false;
    }

    HRESULT hr = device->CreateVertexShader(vsBytecode.data(), vsBytecode.size(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        std::cerr << "[MaterialDX] Failed to create vertex shader.\n";
        return false;
    }

    hr = device->CreatePixelShader(psBytecode.data(), psBytecode.size(), nullptr, &pixelShader);
    if (FAILED(hr)) {
        std::cerr << "[MaterialDX] Failed to create pixel shader.\n";
        return false;
    }

    // For demo, create a simple input layout (e.g. position + texcoord)
    // You should reflect from bytecode or match your vertex format exactly.
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(layoutDesc, 2, vsBytecode.data(), vsBytecode.size(), &inputLayout);
    if (FAILED(hr)) {
        std::cerr << "[MaterialDX] Failed to create input layout.\n";
        return false;
    }

    return true;
}

void MaterialDX::SetTexture(ID3D11ShaderResourceView* textureSRV) {
    texture = textureSRV;
}

void MaterialDX::Bind(ID3D11DeviceContext* context) {
    context->IASetInputLayout(inputLayout.Get());
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
    context->PSSetShaderResources(0, 1, texture.GetAddressOf());
}

bool MaterialDX::LoadShaderBytecode(const std::string& filePath, std::vector<char>& outBytecode) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    outBytecode.resize(size);
    if (!file.read(outBytecode.data(), size)) {
        return false;
    }
    return true;
}
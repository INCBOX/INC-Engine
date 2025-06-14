#include "vk_material.h"
#include <fstream>
#include <iostream>

MaterialVK::~MaterialVK() {
    if (vertShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
    if (fragShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
}

bool MaterialVK::LoadShader(const std::string& vertPath, const std::string& fragPath, VkDevice device) {
    m_device = device;

    std::vector<char> vertCode;
    if (!LoadFile(vertPath, vertCode)) {
        std::cerr << "[MaterialVK] Failed to load vertex shader file: " << vertPath << "\n";
        return false;
    }

    std::vector<char> fragCode;
    if (!LoadFile(fragPath, fragCode)) {
        std::cerr << "[MaterialVK] Failed to load fragment shader file: " << fragPath << "\n";
        return false;
    }

    if (!CreateShaderModule(vertCode, &vertShaderModule)) {
        std::cerr << "[MaterialVK] Failed to create vertex shader module\n";
        return false;
    }

    if (!CreateShaderModule(fragCode, &fragShaderModule)) {
        std::cerr << "[MaterialVK] Failed to create fragment shader module\n";
        return false;
    }

    return true;
}

void MaterialVK::SetTexture(VkImageView imageView, VkSampler sampler) {
    m_imageView = imageView;
    m_sampler = sampler;
}

void MaterialVK::SetDescriptorSet(VkDescriptorSet descriptorSet) {
    m_descriptorSet = descriptorSet;
}

void MaterialVK::Bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout) {
    if (m_descriptorSet == VK_NULL_HANDLE) {
        std::cerr << "[MaterialVK] Warning: Bind called with null descriptor set\n";
        return;
    }

    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                            0, 1, &m_descriptorSet, 0, nullptr);
}

bool MaterialVK::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result = vkCreateShaderModule(m_device, &createInfo, nullptr, shaderModule);
    return (result == VK_SUCCESS);
}

bool MaterialVK::LoadFile(const std::string& filename, std::vector<char>& buffer) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    buffer.resize(size);
    if (!file.read(buffer.data(), size)) {
        return false;
    }
    return true;
}
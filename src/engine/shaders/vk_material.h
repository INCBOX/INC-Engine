#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <vector>

class MaterialVK {
public:
    MaterialVK() = default;
    ~MaterialVK();

    // Initialize shaders from SPIR-V bytecode
    bool LoadShader(const std::string& vertPath, const std::string& fragPath, VkDevice device);

    // Set the Vulkan texture sampler and image view
    void SetTexture(VkImageView imageView, VkSampler sampler);

    // Set the descriptor set handle to be bound during rendering
    void SetDescriptorSet(VkDescriptorSet descriptorSet);

    // Bind pipeline and descriptor sets for rendering
    void Bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout);

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkShaderModule vertShaderModule = VK_NULL_HANDLE;
    VkShaderModule fragShaderModule = VK_NULL_HANDLE;

    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

    // Helper for creating shader modules
    bool CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    // Load SPIR-V shader bytecode from file
    bool LoadFile(const std::string& filename, std::vector<char>& buffer);
};

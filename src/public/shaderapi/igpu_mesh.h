#pragma once
#include <vector>

class IGPUMesh {
public:
    virtual void Upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual size_t GetIndexCount() const = 0;

    virtual ~IGPUMesh() {}
};
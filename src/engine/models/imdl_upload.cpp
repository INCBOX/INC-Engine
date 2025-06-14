// --- engine/models/imdl_model_runtime.cpp ---
#include "models/imdl_cpu.h" //	CPU raw geometry + mesh data
#include "models/imdl_gpu.h" // GPU buffers, index counts
#include "shaderapi.h"
#include <iostream>

bool UploadIMDLToGPU(const IMDL_CPU& data, IMDL_GPU& out) {
    if (!INC::g_pShaderAPI) {
        std::cerr << "[IMDL] ShaderAPI not initialized!\n";
        return false;
    }

    INC::g_pShaderAPI->UploadGeometry(out, data);

    out.indexCount = static_cast<uint32_t>(data.indexData.size());
    out.materialName = data.material;
    return true;
}

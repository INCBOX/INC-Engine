#include "models/imdl_runtime.h"
#include "models/imdl_upload.h"  // UploadIMDLToGPU
#include "shaderapi.h"
#include <iostream>

bool IMDL_RUNTIME::InitFromCPU(const std::shared_ptr<IMDL_CPU>& geom) {
    cpuGeom = geom;

    gpuModel = std::make_shared<IMDL_GPU>();

    if (!UploadIMDLToGPU(*cpuGeom, *gpuModel)) {
        std::cerr << "[IMDL_RUNTIME] Failed to upload GPU model\n";
        gpuModel.reset();
        return false;
    }

    return true;
}

void IMDL_RUNTIME::Render() {
    if (!gpuModel)
        return;

    // TODO: Apply position transform if your shader supports it (MVP etc.)
    INC::g_pShaderAPI->DrawModel(*gpuModel);
}

void IMDL_RUNTIME::Cleanup() {
    // Resources will be freed automatically by shared_ptr, but explicit reset is fine
    gpuModel.reset();
    cpuGeom.reset();
}
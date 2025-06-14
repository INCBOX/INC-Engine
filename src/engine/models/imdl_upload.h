#pragma once

#include "models/imdl_cpu.h" // CPU raw geometry + mesh data
#include "models/imdl_gpu.h" // GPU buffers, index counts

// Change the function prototype to use new types
bool UploadIMDLToGPU(const IMDL_CPU& data, IMDL_GPU& out);

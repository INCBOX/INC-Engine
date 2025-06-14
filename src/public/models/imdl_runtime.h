#pragma once
#include <vector>
#include <memory>
#include "models/imdl_runtime.h"
#include "models/imdl_cpu.h"  				// CPU-side raw geometry
#include "models/imdl_gpu.h"  				// GPU-side buffers
	
struct IMDL_RUNTIME {	
    std::shared_ptr<IMDL_CPU> cpuGeom;  	// CPU model data (renamed to IMDL_CPU)
    std::shared_ptr<IMDL_GPU> gpuModel; 	// GPU buffers and draw info

    float position[3] = {0.0f, 0.0f, 0.0f}; // World position (add rotation/scale later)

    IMDL_RUNTIME() = default;

    // Initialize from CPU geometry: upload to GPU and link
    bool InitFromCPU(const std::shared_ptr<IMDL_CPU>& geom);

    // Render the model instance
    void Render();

    // Cleanup resources if needed
    void Cleanup();
};
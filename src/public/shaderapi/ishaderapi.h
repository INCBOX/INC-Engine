#pragma once

// Abstract interface for all rendering backends (OpenGL, Vulkan, DirectX, etc.)
// This allows the engine to remain backend-agnostic.

class ShaderAPICore {
public:
	virtual ~ShaderAPICore() = default;
	
	virtual void Renderer_Frame(int width, int height) = 0;  // Add this

	// Initialize renderer with platform-specific window handle
	virtual bool Init(void* windowHandle, int width, int height) = 0;

	// Shutdown and release resources
	virtual void Shutdown() = 0;

	// Called at the beginning of each frame (clears, sets viewport, etc.)
	virtual void BeginFrame() = 0;

	// Called at the end of each frame (swap buffers, flush GPU, etc.)
	virtual void EndFrame() = 0;

	// Handle window resize events (optional)
	virtual void OnResize(int width, int height) = 0;
};
#pragma once

// Abstract interface for all rendering backends (OpenGL, Vulkan, DirectX, etc.)
// This allows the engine to remain backend-agnostic.

// JSON GEOMETRY STUFF
class IGeometry;
class Matrix4x4_f;

class ShaderAPICore {
public:
	virtual ~ShaderAPICore() = default;
	
	virtual void PrepareFrame(int width, int height) = 0;

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
	
	// MVP Camera view matrix (world-to-camera)
	virtual void SetViewMatrix(const Matrix4x4_f& viewMatrix) = 0;
	
	// Projection matrix (camera lens)
    virtual void SetProjectionMatrix(const Matrix4x4_f& projMatrix) = 0;
	
	// JSON GEOMETRY Draw a mesh with a transform
	virtual void DrawMesh(const IGeometry& mesh, const Matrix4x4_f& modelMatrix) = 0;

	// Factory to create backend-specific mesh
	virtual IGeometry* CreateMesh() = 0;
};

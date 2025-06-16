#include "shaderapi/shaderapi_gl.h"
#include "shaderapi/shaderapi_gl_shader.h"
#include "shaderapi/shaderapi_gl_buffer.h"
#include "shaderapi/shaderapi_gl_vao.h"
#include "shaderapi/mesh.h"
#include <glad/glad.h>
#include <iostream>
#include "mathlib/matrix.h"





bool ShaderAPI_GL::Init(void* windowHandle, int width, int height) {
    m_Window = static_cast<SDL_Window*>(windowHandle);
    m_GLContext = SDL_GL_CreateContext(m_Window);
    if (!m_GLContext) {
        std::cerr << "[GL] Failed to create GL context\n";
        return false;
    }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[GL] Failed to load GL functions\n";
        return false;
    }

    SDL_GL_SetSwapInterval(1);
    std::cout << "[GL] OpenGL initialized\n";

    glEnable(GL_DEPTH_TEST);

	std::string vertPath = "hl3/shaders/cube.vert";
	std::string fragPath = "hl3/shaders/cube.frag";
	
	m_Shader = std::make_unique<ShaderProgram>();
	if (!m_Shader->CompileFromFile(vertPath.c_str(), fragPath.c_str())) {
		std::cerr << "[GL] Shader compilation failed for: " << vertPath << " and " << fragPath << "\n";
		return false;
	}


    return true;
}


void ShaderAPI_GL::BeginFrame() {
    // Optional: any per-frame setup before rendering
}

void ShaderAPI_GL::EndFrame() {
    DebugDrawUnitCube();
    SDL_GL_SwapWindow(m_Window);
}




void ShaderAPI_GL::SetMVP(const Matrix& mvp) {
    int mvpLoc = glGetUniformLocation(m_Shader->ID, "u_MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.Data());
}

void ShaderAPI_GL::PrepareFrame(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE); // optional
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // solid fill

    // 	Setup camera
    //	Vector cameraPos = Vector(0.0f, 2.0f, 5.0f);       // Position of camera
    //	Vector cameraTarget = Vector(0.0f, 0.0f, 0.0f);    // Look at origin
    //	Vector up = Vector(0.0f, 1.0f, 0.0f);              // World up
	//	
    //	m_ViewMatrix = Matrix::LookAt(cameraPos, cameraTarget, up);
	
	
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    m_ProjMatrix = Matrix::Perspective(30.0f, aspect, 0.1f, 100.0f);
	
		Vector cameraPos(0.0f, 0.0f, 5.0f);
		Vector cameraTarget(0.0f, 0.0f, 0.0f);
		Vector up(0.0f, 1.0f, 0.0f);
		m_ViewMatrix = Matrix::LookAt(cameraPos, cameraTarget, up);



    m_Shader->Use();
}


void ShaderAPI_GL::Shutdown() {
    if (m_Shader) {
        m_Shader->Delete();
        m_Shader.reset();
    }

    // m_VBO and m_VAO no longer members, so do NOT reset here

    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }
}


void ShaderAPI_GL::OnResize(int width, int height) {
    glViewport(0, 0, width, height);
}


// MESH
void ShaderAPI_GL::DrawMesh(const Mesh& mesh, const Matrix& modelMatrix) {
    m_Shader->Use();

    // Compute final MVP matrix (viewProjection * model)
    // For now, assume identity viewProjection; you can expand this later
    Matrix mvp = m_ProjMatrix * m_ViewMatrix * modelMatrix;
    SetMVP(mvp);

    mesh.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
    mesh.Unbind();
}

// CAMERA STUFF
void ShaderAPI_GL::SetViewMatrix(const Matrix& viewMatrix)
{
    m_ViewMatrix = viewMatrix;
}


void ShaderAPI_GL::DebugDrawUnitCube() {
    std::vector<float> vertices = {
        -0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f, 0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,   0.5f,-0.5f, 0.5f,   0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f
    };
    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0,  // back
        4,5,6, 6,7,4,  // front
        0,4,7, 7,3,0,  // left
        1,5,6, 6,2,1,  // right
        3,2,6, 6,7,3,  // top
        0,1,5, 5,4,0   // bottom
    };

    Mesh tempCube;
    tempCube.Upload(vertices, indices);

    Matrix model = Matrix::Identity();
    Matrix mvp = m_ProjMatrix * m_ViewMatrix * model;
    SetMVP(mvp);

    // 👇 Add this debug print
    std::cout << "[DEBUG] MVP Matrix:\n";
    const float* data = mvp.Data();
    for (int i = 0; i < 4; ++i)
        std::cout << data[i * 4] << " " << data[i * 4 + 1] << " " << data[i * 4 + 2] << " " << data[i * 4 + 3] << "\n";

    tempCube.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(tempCube.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
    tempCube.Unbind();
}

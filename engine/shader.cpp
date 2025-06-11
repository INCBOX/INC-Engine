#include "shader.h"

Shader::Shader() : programID(0) {}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

bool Shader::Load(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode, fragmentCode;
    if (!ReadFile(vertexPath, vertexCode) || !ReadFile(fragmentPath, fragmentCode)) {
        std::cerr << "Failed to load shader files.\n";
        return false;
    }
    return CreateProgram(vertexCode.c_str(), fragmentCode.c_str());
}

void Shader::Bind() const {
    glUseProgram(programID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

GLuint Shader::GetProgram() const {
    return programID;
}

void Shader::SetUniformMat4(const std::string& name, const float* matrix) {
    GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
    }
}

void Shader::SetUniformVec3(const std::string& name, float x, float y, float z) {
    GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform3f(loc, x, y, z);
    }
}

void Shader::SetUniformInt(const std::string& name, int value) {
    GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform1i(loc, value);
    }
}

void Shader::SetUniformFloat(const std::string& name, float value) {
    GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform1f(loc, value);
    }
}

bool Shader::ReadFile(const std::string& path, std::string& outCode) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file: " << path << std::endl;
        return false;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    outCode = ss.str();
    return true;
}

GLuint Shader::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n"
                  << infoLog << "\n";
        return 0;
    }
    return shader;
}

bool Shader::CreateProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    if (vertexShader == 0) return false;

    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

GLint Shader::GetUniformLocation(const std::string& name) {
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];

    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or is not used!\n";
    }
    uniformLocationCache[name] = location;
    return location;
}

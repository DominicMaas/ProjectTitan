#pragma once

#include "pch.h"

class Shader {
private:
    // Source code for the shaders
    std::vector<char> _vertexSource;
    std::vector<char> _fragmentSource;

    // Loads a compiled shader from the system into memory
    static std::vector<char> loadShader(const char *path);
public:
    // constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);

    std::vector<char> getVertexSource() {
        return _vertexSource;
    }

    std::vector<char> getFragmentSource() {
        return _fragmentSource;
    }
};

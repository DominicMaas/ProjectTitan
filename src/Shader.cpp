#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    _vertexSource = loadShader(vertexPath);
    _fragmentSource = loadShader(fragmentPath);
}

std::vector<char> Shader::loadShader(const char *path) {
    // Open the file
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    // Ensure the file exists and is open
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    // Determine the size of the file
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    // Go back to the start of the file and read everything into the buffer
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}




void Shader::use() {

}

unsigned int Shader::getId() {
    return 0;
}

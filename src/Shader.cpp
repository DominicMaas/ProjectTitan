#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

unsigned int Shader::getId() {
    return _id;
}

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    GLCall(glShaderSource(vertex, 1, &vShaderCode, NULL));
    GLCall(glCompileShader(vertex));

    // print compile errors if any
    GLCall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &success));
    if (!success) {
        GLCall(glGetShaderInfoLog(vertex, 512, NULL, infoLog));
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(fragment, 1, &fShaderCode, NULL));
    GLCall(glCompileShader(fragment));

    // print compile errors if any
    GLCall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &success));
    if (!success) {
        GLCall(glGetShaderInfoLog(fragment, 512, NULL, infoLog));
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // shader Program
    _id = glCreateProgram();
    GLCall(glAttachShader(_id, vertex));
    GLCall(glAttachShader(_id, fragment));
    GLCall(glLinkProgram(_id));

    // print linking errors if any
    GLCall(glGetProgramiv(_id, GL_LINK_STATUS, &success));
    if (!success) {
        GLCall(glGetProgramInfoLog(_id, 512, NULL, infoLog));
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    GLCall(glDeleteShader(vertex));
    GLCall(glDeleteShader(fragment));
}

void Shader::use() {
    GLCall(glUseProgram(_id));
}
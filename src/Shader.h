#pragma once

#include "pch.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Window.h"

class Shader {
private:
    // Source code for the shaders
    std::vector<char> _vertexSource;
    std::vector<char> _fragmentSource;

    // Loads a compiled shader from the system into memory
    std::vector<char> loadShader(const char *path);
public:
    std::vector<char> getVertexSource() {
        return _vertexSource;
    }

    std::vector<char> getFragmentSource() {
        return _fragmentSource;
    }



    // OLD


    // Return the shader program ID
    unsigned int getId();

    // constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);

    //Shader(Window* window, const std::vector<char>& vertexSource, const std::vector<char>& fragmentSource);
    //~Shader();

    // use/activate the shader
    void use();

    // utility uniform functions
    void setColor(const std::string &name, glm::vec3 color) {
        //GLCall(glUniform3f(glGetUniformLocation(_id, name.c_str()), color.x, color.y, color.z));
    }

    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const {
        //GLCall(glUniform1i(glGetUniformLocation(_id, name.c_str()), (int) value));
    }

    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const {
        //GLCall(glUniform1i(glGetUniformLocation(_id, name.c_str()), value));
    }

    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const {
        //GLCall(glUniform1f(glGetUniformLocation(_id, name.c_str()), value));
    }

    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const {
        //GLCall(glUniform2fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]));
    }

    void setVec2(const std::string &name, float x, float y) const {
        //GLCall(glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y));
    }

    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        //GLCall(glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]));
    }

    void setVec3(const std::string &name, float x, float y, float z) const {
        //GLCall(glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z));
    }

    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const {
        //GLCall(glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]));
    }

    void setVec4(const std::string &name, float x, float y, float z, float w) const {
        //GLCall(glUniform4f(glGetUniformLocation(_id, name.c_str()), x, y, z, w));
    }

    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        //GLCall(glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        //GLCall(glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        //GLCall(glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }
};

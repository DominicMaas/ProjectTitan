#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <constants.h>

class Shader {
private:
    unsigned int _id;

public:
    // Return the shader program ID
    unsigned int getId();

    // constructor reads and builds the shader
    Shader(const GLchar *vertexPath, const GLchar *fragmentPath);

    // use/activate the shader
    void use();

    // utility uniform functions
    void setColor(const std::string &name, glm::vec3 color) {
        GLCall(glUniform3f(glGetUniformLocation(_id, name.c_str()), color.x, color.y, color.z));
    }

    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const {
        GLCall(glUniform1i(glGetUniformLocation(_id, name.c_str()), (int) value));
    }

    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const {
        GLCall(glUniform1i(glGetUniformLocation(_id, name.c_str()), value));
    }

    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const {
        GLCall(glUniform1f(glGetUniformLocation(_id, name.c_str()), value));
    }

    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const {
        GLCall(glUniform2fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]));
    }

    void setVec2(const std::string &name, float x, float y) const {
        GLCall(glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y));
    }

    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        GLCall(glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]));
    }

    void setVec3(const std::string &name, float x, float y, float z) const {
        GLCall(glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z));
    }

    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const {
        GLCall(glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]));
    }

    void setVec4(const std::string &name, float x, float y, float z, float w) const {
        GLCall(glUniform4f(glGetUniformLocation(_id, name.c_str()), x, y, z, w));
    }

    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        GLCall(glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        GLCall(glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        GLCall(glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }
};

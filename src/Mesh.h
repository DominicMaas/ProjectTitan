#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Mesh {
private:
    unsigned int _vbo;
    unsigned int _vao;

    std::vector<glm::vec3> _vertices;

public:
    Mesh(std::vector<glm::vec3> vertices);
    ~Mesh();

    void render();
};
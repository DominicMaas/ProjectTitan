#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
private:
    unsigned int _vao, _vbo, _ebo;

    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;

    bool _hasIndices;
    bool _built;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    ~Mesh();

    // Builds the mesh for rendering. This can only be called once,
    // delete the mesh and create a new one to rebuild it.
    void build();

    // Renders the mesh to the display, the mesh
    // must be built first
    void render(Shader &shader);
};
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"

struct Vertex {
    Vertex() {}

    Vertex(glm::vec3 position) {
        Position = position;
    }

    Vertex(glm::vec3 position, glm::vec3 normal) {
        Position = position;
        Normal = normal;
    }

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) {
        Position = position;
        Normal = normal;
        TexCoords = texCoords;
    }

    Vertex(int px, int py, int pz, int nx, int ny, int nz, glm::vec2 texCoords) {
        Position = glm::vec3(px, py, pz);
        Normal = glm::vec3(nx, ny, nz);
        TexCoords = texCoords;
    }

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
    bool _built;

public:
    // Create a new mesh with a set of vertices, indices and textures. The mesh will not be built
    // until build() is called.
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    // Create a new empty mesh, make sure you call rebuild() to place vertices, indices and textures within
    // the mesh at a later time.
    Mesh();
    ~Mesh();

    // Rebuilds the mesh with a new set of vertices, indices and textures
    void rebuild(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    // Builds the mesh for rendering.
    void build();

    // Renders the mesh to the display, the mesh
    // must be built first
    void render(Shader &shader);

    // If this mesh has been built
    bool isBuilt() const { return _built; }

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture> Textures;
};
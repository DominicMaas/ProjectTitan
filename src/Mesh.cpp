#include <constants.h>
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->_vertices = vertices;
    this->_indices = indices;
    this->_textures = textures;

    this->_hasIndices = !this->_indices.empty();
    this->_built = false;
}

Mesh::~Mesh() {
    if (_built) {
        GLCall(glDeleteVertexArrays(1, &_vao));
        GLCall(glDeleteBuffers(1, &_ebo));
        GLCall(glDeleteBuffers(1, &_vbo));
    }
}

void Mesh::build() {
    // Can only build once
    if (_built) {
        spdlog::warn("[Mesh] Attempted to build a mesh, but it has already been built");
        return;
    }

    // Generate the arrays and buffers
    GLCall(glGenVertexArrays(1, &_vao));
    GLCall(glGenBuffers(1, &_vbo));
    GLCall(glGenBuffers(1, &_ebo));

    // Bind to build
    GLCall(glBindVertexArray(_vao));

    // Bind vertex buffer data
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW));

    // Bind index buffer data
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW));

    // Enable shader attributes
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position)));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal)));
    GLCall(glEnableVertexAttribArray(1));

    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords)));
    GLCall(glEnableVertexAttribArray(2));

    // Unbind as built
    GLCall(glBindVertexArray(0));

    _built = true;
}

void Mesh::render(Shader &shader) {
    // Only render if the mesh has been built
    if (!_built) {
        spdlog::warn("[Mesh] Attempted to render mesh before it was built");
        return;
    }

    // Textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < _textures.size(); i++) {
        GLCall(glActiveTexture(GL_TEXTURE0 + i)); // activate proper texture unit before binding

        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = _textures[i].type;

        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        }

        shader.setFloat(("material." + name + number).c_str(), i);
        GLCall(glBindTexture(GL_TEXTURE_2D, _textures[i].id));
    }

    // Draw mesh
    GLCall(glBindVertexArray(_vao));

    // Draw from the element buffer
    //if (_hasIndices) {
    //    GLCall(glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0));
    //} else { // Draw from the array buffer
        GLCall(glDrawArrays(GL_TRIANGLES, 0, _vertices.size()));
    //}

    // Cleanup
    GLCall(glActiveTexture(GL_TEXTURE0));
}


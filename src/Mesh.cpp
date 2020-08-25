#include "Mesh.h"

Mesh::Mesh(std::vector<glm::vec3> vertices) {
    // Set class properties
    this->_vertices = vertices;

    // Generate the arrays and buffers
    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    // Bind to build
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Bind buffer data
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

    // Enable shader attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) 0);
    glEnableVertexAttribArray(0);

    // Unbind as built
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindVertexArray(_vao);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

void Mesh::render() {
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
}
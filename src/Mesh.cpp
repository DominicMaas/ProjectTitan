#include <constants.h>
#include "Mesh.h"

Mesh::Mesh(std::vector<glm::vec3> vertices) {
    // Set class properties
    this->_vertices = vertices;

    // Generate the arrays and buffers
    GLCall(glGenBuffers(1, &_vbo));
    GLCall(glGenVertexArrays(1, &_vao));

    // Bind to build
    GLCall(glBindVertexArray(_vao));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));

    // Bind buffer data
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW));

    // Enable shader attributes
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) 0));
    GLCall(glEnableVertexAttribArray(0));

    // Unbind as built
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    GLCall(glBindVertexArray(_vao));
}

Mesh::~Mesh() {
    GLCall(glDeleteVertexArrays(1, &_vao));
    GLCall(glDeleteBuffers(1, &_vbo));
}

void Mesh::render() {
    GLCall(glBindVertexArray(_vao));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, _vertices.size()));
}
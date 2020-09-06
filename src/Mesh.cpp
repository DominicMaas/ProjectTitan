#include "Mesh.h"

Mesh::Mesh() {
    this->Vertices = std::vector<Vertex>();
    this->Indices = std::vector<unsigned int>();
    this->Textures = std::vector<Texture>();

    this->_built = false;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;
}

Mesh::~Mesh() {
    // Delete the arrays and buffers
    //GLCall(glDeleteVertexArrays(1, &_vao));
    //GLCall(glDeleteBuffers(1, &_ebo));
    //GLCall(glDeleteBuffers(1, &_vbo));
}

void Mesh::rebuild(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->Vertices = vertices;
    this->Indices = indices;
    this->Textures = textures;

    this->_built = false;

    //build(nullptr);
}

void Mesh::build(VmaAllocator &allocator) {
    // If the mesh has already been built, we need to destory it first
    if (_built) {
        destroy(allocator);
    }

    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = sizeof(Vertex) * Vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer tempBuffer;
    if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &tempBuffer, &_allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer!");
    }

    // Convert to the c++ object
    _vertexBuffer = tempBuffer;

    // What in the actual fuck is Vulkan
    void* data;
    vmaMapMemory(allocator, _allocation, &data);
    memcpy(data, Vertices.data(), (size_t) bufferInfo.size);
    vmaUnmapMemory(allocator, _allocation);

    // Bind to build
    //GLCall(glBindVertexArray(_vao));

    // Bind vertex buffer data
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    //GLCall(glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW));

    // Bind index buffer data
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo));
    //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW));

    // Enable shader attributes
    //GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position)));
    //GLCall(glEnableVertexAttribArray(0));

    //GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal)));
    //GLCall(glEnableVertexAttribArray(1));

    //GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords)));
    //GLCall(glEnableVertexAttribArray(2));

    // Unbind as built
    //GLCall(glBindVertexArray(0));

    _built = true;
}

void Mesh::destroy(VmaAllocator &allocator) {
    vmaDestroyBuffer(allocator, _vertexBuffer, _allocation);
}

void Mesh::render(vk::CommandBuffer commandBuffer) {
    // Only render if the mesh has been built
    if (!_built) {
        spdlog::warn("[Mesh] Attempted to render mesh before it was built");
        return;
    }

    // Bind
    vk::Buffer vertexBuffers[] = { _vertexBuffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

    // Draw
    commandBuffer.draw(Vertices.size(), 1, 0, 0);

    // Textures
    /*unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < Textures.size(); i++) {
        GLCall(glActiveTexture(GL_TEXTURE0 + i)); // activate proper texture unit before binding

        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = Textures[i].type;

        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        }

        shader.setFloat(("material." + name + number).c_str(), i);
        GLCall(glBindTexture(GL_TEXTURE_2D, Textures[i].id));
    }

    // Draw mesh
    GLCall(glBindVertexArray(_vao));

    // Draw from the element buffer
    if (!Indices.empty()) {
        GLCall(glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0));
    } else { // Draw from the array buffer
        GLCall(glDrawArrays(GL_TRIANGLES, 0, Vertices.size()));
    }

    // Cleanup
    GLCall(glActiveTexture(GL_TEXTURE0));*/
}



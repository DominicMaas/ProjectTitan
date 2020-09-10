#pragma once

#include <pch.h>
#include "GraphicsPipeline.h"
#include "core/Renderable.h"
#include "core/Vertex.h"

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh: public Renderable {
private:
    vk::Buffer _vertexBuffer;
    vk::Buffer _indexBuffer;
    vk::Buffer _uniformBuffer;

    VmaAllocation _vertexAllocation;
    VmaAllocation _indexAllocation;
    VmaAllocation _uniformAllocation;

    vk::DescriptorSet _descriptorSet;

    std::string _pipelineName;

    bool _built;

public:
    // Create a new mesh with a set of vertices, indices and textures. The mesh will not be built
    // until build() is called.
    Mesh(const std::string& pipelineName, std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::vector<Texture> textures);

    // Create a new empty mesh, make sure you call rebuild() to place vertices, indices and textures within
    // the mesh at a later time.
    Mesh(const std::string& pipelineName);

    // Rebuilds the mesh with a new set of vertices, indices and textures
    void rebuild(std::vector<Vertex> vertices, std::vector<unsigned short> indices, std::vector<Texture> textures, RenderableData input);

    void build(RenderableData input) override;
    void render(vk::CommandBuffer &commandBuffer, GraphicsPipeline &pipeline) override;
    void update(RenderableData input, long double deltaTime) override;
    void destroy(RenderableData input) override;

    // If this mesh has been built
    [[nodiscard]] bool isBuilt() const { return _built; }

    std::vector<Vertex> Vertices;
    std::vector<unsigned short> Indices;
    std::vector<Texture> Textures;
};
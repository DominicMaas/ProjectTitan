#pragma once

#include "../pch.h"

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

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription = {
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = vk::VertexInputRate::eVertex
        };

        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions;

        // Position
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat; // vec3;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);

        // Normal
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat; // vec3;
        attributeDescriptions[1].offset = offsetof(Vertex, Normal);

        // TexCoords
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat; // vec2
        attributeDescriptions[2].offset = offsetof(Vertex, TexCoords);

        return attributeDescriptions;
    }
};
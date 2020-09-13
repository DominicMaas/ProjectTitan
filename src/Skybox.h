#pragma once

#include <pch.h>
#include "core/Mesh.h"
#include "core/resources/Texture2D.h"

class Skybox {
private:
    Mesh *_mesh;
    Texture2D *_texture;
    GraphicsPipeline *_pipeline;

    vk::Buffer _uniformBuffer;
    VmaAllocation _uniformAllocation;
    vk::DescriptorSet _descriptorSet;

    const Vertex _skyboxVertices[36] = {
        // positions
        Vertex { glm::vec3(-1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, -1.0f) },

        Vertex { glm::vec3(-1.0f, -1.0f, 1.0f) },
        Vertex { glm::vec3(-1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(-1.0f, -1.0f, 1.0f) },

        Vertex { glm::vec3(1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, -1.0f) },

        Vertex { glm::vec3(-1.0f, -1.0f, 1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, 1.0f) },
        Vertex { glm::vec3(-1.0f, -1.0f, 1.0f) },

        Vertex { glm::vec3(-1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, 1.0f) },
        Vertex { glm::vec3(-1.0f, 1.0f, -1.0f) },

        Vertex { glm::vec3(-1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, -1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, -1.0f) },
        Vertex { glm::vec3(-1.0f, -1.0f, 1.0f) },
        Vertex { glm::vec3(1.0f, -1.0f, 1.0f) },
    };

public:
    Skybox();
    ~Skybox();

    void render(vk::CommandBuffer &commandBuffer, glm::mat4 viewMatrix, glm::mat4 projMatrix);
};
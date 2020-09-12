#pragma once

#include <pch.h>
#include "core/managers/ResourceManager.h"
#include "core/Mesh.h"

class Skybox {
private:
    Mesh _mesh;

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
    ~Skybox();

    void setup(std::vector<std::string> faces);
    void render(glm::mat4 viewMatrix, glm::mat4 projMatrix);
};
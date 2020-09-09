#pragma once

#include "pch.h"
#include "Shader.h"
#include <stb_image.h>
#include "core/ResourceManager.h"
#include "Mesh.h"

class Skybox {
private:
    unsigned int _texture;

    Mesh _mesh;
    Shader* _shader;

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
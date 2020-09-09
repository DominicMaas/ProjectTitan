#include <constants.h>
#include "Skybox.h"
#include "iostream"

void Skybox::setup(std::vector<std::string> faces) {
    // Get the shader
    _shader = ResourceManager::getShader("skybox");

    // Build the mesh
    std::vector<Vertex> vertices(std::begin(_skyboxVertices), std::end(_skyboxVertices));
    //_mesh.rebuild(vertices, std::vector<unsigned int>(), std::vector<Texture>());

    // Generate textures
    GLCall(glGenTextures(1, &_texture));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, _texture));

    // Load textures
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        stbi_set_flip_vertically_on_load(true);
        if (data) {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            ));
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Set texture settings
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    //_shader->use();
   // _shader->setInt("skybox", 0);
}

void Skybox::render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    GLCall(glDepthFunc(GL_LEQUAL));

    //_shader->use();

    // Keep skybox in player view
    glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));

    // Set the camera view and view position matrix
    //_shader->setMat4("view", view);
    //_shader->setMat4("projection", projMatrix);

    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, _texture));
    //_mesh.render(*_shader);
}

Skybox::~Skybox() {
    glDeleteTextures(1, &_texture);
}

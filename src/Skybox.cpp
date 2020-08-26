#include <constants.h>
#include "Skybox.h"

Skybox::Skybox(std::string shaderName) {
    this->_shaderName = shaderName;
}

void Skybox::setup(std::vector<std::string> faces) {
    // Build the skybox cube
    GLCall(glGenVertexArrays(1, &_vao));
    GLCall(glGenBuffers(1, &_vbo));

    // Work with this vao
    GLCall(glBindVertexArray(_vao));

    // Bind vertices
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(_skyboxVertices), _skyboxVertices, GL_STATIC_DRAW));

    // position attribute
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0));
    GLCall(glEnableVertexAttribArray(0));

    // Generate textures
    GLCall(glGenTextures(1, &_texture));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, _texture));

    // Load textures
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
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

    ResourceManager::getShader(_shaderName)->use();
    ResourceManager::getShader(_shaderName)->setInt("skybox", 0);
}

void Skybox::render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    GLCall(glDepthFunc(GL_LEQUAL));

    Shader* shader = ResourceManager::getShader(_shaderName);
    shader->use();

    // Keep skybox in player view
    glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));

    // Set the camera view and view position matrix
    shader->setMat4("view", view);
    shader->setMat4("projection", projMatrix);

    GLCall(glBindVertexArray(_vao));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, _texture));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
    GLCall(glDepthFunc(GL_LESS));
}
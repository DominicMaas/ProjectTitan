#include "Skybox.h"

Skybox::Skybox(Shader shader) : _shader(shader) {}

void Skybox::setup(std::vector<std::string> faces) {
    // Build the skybox cube
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    // Work with this vao
    glBindVertexArray(_vao);

    // Bind vertices
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_skyboxVertices), _skyboxVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Generate textures
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

    // Load textures
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Set texture settings
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    _shader.use();
    _shader.setInt("skybox", 0);
}

void Skybox::render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    glDepthFunc(GL_LEQUAL);
    _shader.use();

    // Keep skybox in player view
    glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));

    // Set the camera view and view position matrix
    _shader.setMat4("view", view);
    _shader.setMat4("projection", projMatrix);

    glBindVertexArray(_vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}
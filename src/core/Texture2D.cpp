#include "Texture2D.h"

Texture2D::Texture2D() {
    // Generate the resource
    glGenTextures(1, &this->_resourceId);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &this->_resourceId);
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, this->_resourceId);
}

void Texture2D::load(unsigned char *data, int width, int height) {
    // Bind to this resource and start building it
    glBindTexture(GL_TEXTURE_2D, this->_resourceId);

    // set the texture wrapping / filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Finished
    glBindTexture(GL_TEXTURE_2D, 0);
}

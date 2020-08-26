#include <constants.h>
#include "Texture2D.h"

Texture2D::Texture2D() {
    // Generate the resource
    GLCall(glGenTextures(1, &this->_textureId));
}

Texture2D::~Texture2D() {
    GLCall(glDeleteTextures(1, &this->_textureId));
}

void Texture2D::bind() const {
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, this->_textureId));
}

void Texture2D::load(unsigned char *data, int width, int height) {
    this->_width = width;
    this->_height = height;

    // Bind to this resource and start building it
    GLCall(glBindTexture(GL_TEXTURE_2D, this->_textureId));

    // set the texture wrapping / filtering options
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Load the texture
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));

    // Finished
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

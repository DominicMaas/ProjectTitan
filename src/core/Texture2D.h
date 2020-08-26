#pragma once

#include <glad/glad.h>

class Texture2D {
private:
    unsigned int _resourceId;

public:
    Texture2D();
    ~Texture2D();

    void load(unsigned char *data, int width, int height);
    void bind() const;
};

#pragma once

#include <pch.h>

class Texture2D {
private:
    unsigned int _textureId;
    int _width;
    int _height;

public:
    Texture2D();
    ~Texture2D();

    void load(unsigned char *data, int width, int height, int wrap, int filter);
    void bind() const;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
};

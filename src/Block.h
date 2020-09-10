#pragma once

#include <pch.h>

class Block {
private:
    unsigned int _material = 0;
    float _isoValue;

public:
    float getIsoValue() { return _isoValue; }

    void setIsoValue(float isoValue) { _isoValue = isoValue; }

    unsigned int getMaterial() { return _material; };

    void setMaterial(unsigned int material) { _material = material; }
};

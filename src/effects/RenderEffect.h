#pragma once

#include "../pch.h"
#include "../Camera.h"

class RenderEffect {
public:
    // Render the effect to the screen
    virtual void render(Camera *camera) {};
};
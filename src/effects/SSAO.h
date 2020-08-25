#pragma once

#include "RenderEffect.h"

class SSAO : public RenderEffect {
public:
    void render(Camera *camera);
};
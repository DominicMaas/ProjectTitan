#pragma once

#include "../pch.h"
#include "RenderEffect.h"

class ShadowMapping : public RenderEffect {
public:
    ShadowMapping();
    ~ShadowMapping();

    void renderToDepthMap();
    void finishRenderingToDepthMap(int width, int height);

    void renderDebugQuad();

    static const int SHADOW_WIDTH = 1024;
    static const int SHADOW_HEIGHT = 1024;

    constexpr static const float NEAR_PLANE = 1.0f;
    constexpr static const float FAR_PLANE = 1000.0f;

private:
    unsigned int _frameBuffer;
    unsigned int _depthTexture;
};
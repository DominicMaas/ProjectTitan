#include <constants.h>
#include "ShadowMapping.h"

ShadowMapping::ShadowMapping(int width, int height) {
    // Generate the depth map texture
    GLCall(glGenTextures(1, &_depthMap));
    GLCall(glBindTexture(GL_TEXTURE_2D, _depthMap));

    // Set the required paramters
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

    // Set border color
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

    // attach depth texture as FBO's depth buffer
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0));
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

ShadowMapping::~ShadowMapping() {
    GLCall(glDeleteTextures(1, &_depthMap));
}

void ShadowMapping::render(Camera *camera) {
    // 1. Render depth of scene to texture (from light's perspective)

    // 2. Render scene as normal using the generated depth / shadow map
}
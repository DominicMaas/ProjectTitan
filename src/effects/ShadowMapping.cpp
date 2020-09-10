#include "ShadowMapping.h"
#include "../Shader.h"
#include "../core/managers/ResourceManager.h"

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        //glGenVertexArrays(1, &quadVAO);
        //glGenBuffers(1, &quadVBO);
        //glBindVertexArray(quadVAO);
        //glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        //glEnableVertexAttribArray(0);
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        //glEnableVertexAttribArray(1);
        ///glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    //glBindVertexArray(quadVAO);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glBindVertexArray(0);
}

ShadowMapping::ShadowMapping() {
    // Generate the depth map fbo
    //GLCall(glGenFramebuffers(1, &_frameBuffer));

    // create depth texture
    //glGenTextures(1, &_depthTexture);
    //glBindTexture(GL_TEXTURE_2D, _depthTexture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture as FBO's depth buffer
    //glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
    //glDrawBuffer(GL_NONE);
    ///glReadBuffer(GL_NONE);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMapping::~ShadowMapping() {
    //GLCall(glDeleteTextures(1, &_depthTexture));
    //GLCall(glDeleteFramebuffers(1, &_frameBuffer));
}

void ShadowMapping::finishRenderingToDepthMap(int width, int height) {
    // Finish the bind buffer
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport
    //glViewport(0, 0, width, height);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, _depthTexture);
}

void ShadowMapping::renderToDepthMap() {
    // 1. first render to depth map
    //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    //glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    //glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMapping::renderDebugQuad() {
   // Shader* debugDepthQuadShader = ResourceManager::getShader("debug_depth_quad");
    //debugDepthQuadShader->use();
    //debugDepthQuadShader->setFloat("near_plane", NEAR_PLANE);
    //debugDepthQuadShader->setFloat("far_plane", FAR_PLANE);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, _depthTexture);
    //renderQuad();
}

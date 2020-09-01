#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

static const int CHUNK_HEIGHT = 64;
static const int CHUNK_WIDTH = 16;

#define GLCall(x) GLClearError(); x;\
    GLLogCall(#x, __FILE__, __LINE__)

static void GLClearError() {
    while(glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        spdlog::error("[OpenGL Error] ({}) {} {}:{}", error, function, file, line);
        return false;
    }

    return true;
}

enum ColliderCategory {
    COLLIDER_GENERAL = 0x0001,
    COLLIDER_WORLD_GROUND = 0x0002
};
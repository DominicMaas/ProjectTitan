#pragma once

#include <stb_image.h>

#include <map>
#include <string>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "Texture2D.h"
#include "../Shader.h"

class ResourceManager {
private:
private:
    ResourceManager() = default; // Private constructor

    static std::map<std::string, Shader*> _shaders;
    static std::map<std::string, Texture2D*> _textures;

public:
    // Loads a shader into the resource manager, do not include
    // the path extension to the shader, the resource manager will
    // load all appropriate shaders
    static void loadShader(std::string name, std::string path);

    // Loads a texture into the resource manager
    static void loadTexture(std::string name, std::string path);

    // Get a shader of the specified name
    static Shader* getShader(std::string name);

    // Get a texture of the specified name
    static Texture2D* getTexture(std::string name);

    // Removes all resources from the resource manager, call this
    // when the game is closing
    static void clean();
};
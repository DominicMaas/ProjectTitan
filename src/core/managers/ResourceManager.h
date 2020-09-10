#pragma once

#include <pch.h>
#include "../../Shader.h"
#include "../Model.h"
#include "../Texture2D.h"

class Model;
struct RenderableData;

class ResourceManager {
private:
private:
    ResourceManager() = default; // Private constructor

    static std::map<std::string, Shader*> _shaders;
    static std::map<std::string, Texture2D*> _textures;
    static std::map<std::string, Model*> _models;

public:
    // Loads a shader into the resource manager, do not include
    // the path extension to the shader, the resource manager will
    // load all appropriate shaders
    static void loadShader(std::string name, std::string path);

    // Loads a texture into the resource manager
    static void loadTexture(std::string name, std::string path);

    // Loads a model into the resource manager
    static void loadModel(std::string name, std::string path, RenderableData data);

    // Get a shader of the specified name
    static Shader* getShader(std::string name);

    // Get a texture of the specified name
    static Texture2D* getTexture(std::string name);

    // Get a model of the specified name
    static Model* getModel(std::string name);

    // Removes all resources from the resource manager, call this
    // when the game is closing
    static void clean();
};
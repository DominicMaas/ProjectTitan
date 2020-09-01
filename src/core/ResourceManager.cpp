#include "ResourceManager.h"

// Instantiate static variables
std::map<std::string, Shader*> ResourceManager::_shaders;
//std::map<std::string, Texture2D*> ResourceManager::_textures;
//std::map<std::string, Model*> ResourceManager::_models;

void ResourceManager::loadShader(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading shader '" + name + "'...");
    _shaders[name] = new Shader(std::string(path + ".vert.spv").c_str(), std::string(path + ".frag.spv").c_str());
}

Shader* ResourceManager::getShader(std::string name) {
    return _shaders[name];
}

/*void ResourceManager::loadTexture(std::string name, std::string path, int wrap, int filter) {
    spdlog::info("[Resource Manager] Loading texture '" + name + "'...");

    // The texture the image will be stored in
    auto* texture = new Texture2D();

    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

    // If successful
    if (data) {
        // Load in the texture
        texture->load(data, width, height, wrap, filter);
        _textures[name] = texture;
    } else {
        spdlog::error("[Resource Manager] Could not load texture!");
        delete texture;
    }

    // Free image resources
    stbi_image_free(data);
}

void ResourceManager::loadModel(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading model '" + name + "'...");

    Model* model = new Model(path.c_str());
    model->build();

    _models[name] = model;
}

Shader* ResourceManager::getShader(std::string name) {
    return _shaders[name];
}

Texture2D* ResourceManager::getTexture(std::string name) {
    return _textures[name];
}

Model *ResourceManager::getModel(std::string name) {
    return _models[name];
}
*/
void ResourceManager::clean() {

}

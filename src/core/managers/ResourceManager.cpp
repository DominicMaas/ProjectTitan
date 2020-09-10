#include "ResourceManager.h"

// Instantiate static variables
std::map<std::string, Shader*> ResourceManager::_shaders;
std::map<std::string, Texture2D*> ResourceManager::_textures;
std::map<std::string, Model*> ResourceManager::_models;

void ResourceManager::loadShader(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading shader '" + name + "'...");
    _shaders[name] = new Shader(std::string(path + ".vert.spv").c_str(), std::string(path + ".frag.spv").c_str());
}

Shader* ResourceManager::getShader(std::string name) {
    return _shaders[name];
}

void ResourceManager::loadTexture(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading texture '" + name + "'...");

    // The texture the image will be stored in
    auto* texture = new Texture2D();

    // load image
    int width, height, texChannels;
    unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);

    // If successful
    if (pixels) {
        // Load in the texture
        texture->load(pixels, width, height);
        _textures[name] = texture;
    } else {
        spdlog::error("[Resource Manager] Could not load texture!");
        delete texture;
    }

    // Free image resources
    stbi_image_free(pixels);
}

Texture2D* ResourceManager::getTexture(std::string name) {
    return _textures[name];
}

void ResourceManager::loadModel(std::string name, std::string path, RenderableData data) {
    spdlog::info("[Resource Manager] Loading model '" + name + "'...");

    Model* model = new Model(path.c_str());
    model->build(data);

    _models[name] = model;
}

Model *ResourceManager::getModel(std::string name) {
    return _models[name];
}

void ResourceManager::clean() {

}

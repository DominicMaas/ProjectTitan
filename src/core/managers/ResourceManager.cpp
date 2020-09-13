#include "ResourceManager.h"

// Instantiate static variables
boost::ptr_map<std::string, Shader> ResourceManager::_shaders;
boost::ptr_map<std::string, Texture2D> ResourceManager::_textures;
boost::ptr_map<std::string, Model> ResourceManager::_models;

void ResourceManager::loadShader(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading shader '" + name + "'...");

    if (_shaders.find(name) == _shaders.end()) {
        auto* shader = new Shader(std::string(path + ".vert.spv").c_str(), std::string(path + ".frag.spv").c_str());
        _shaders.insert(name, shader);
    } else {
        spdlog::error("[Resource Manager] Could not load shader, a shader of this name already exists.");
    }
}

void ResourceManager::loadTexture(std::string name, std::vector<std::string> paths, LoadTextureInfo info) {
    spdlog::info("[Resource Manager] Loading texture '" + name + "'...");

    if (_textures.find(name) != _textures.end()) {
        spdlog::error("[Resource Manager] Could not model texture, a texture of this name already exists.");
        return;
    }

    // The texture the image will be stored in
    auto* texture = new Texture2D();

    // Set how the image should be loaded
    stbi_set_flip_vertically_on_load(info.flipTexture);

    std::vector<unsigned char*> inputTextures;
    int width, height, texChannels;
    bool success = true;

    // Load the images
    for (std::string &path : paths) {
        // Load this image
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);

        // If successful
        if (pixels) {
            inputTextures.push_back(pixels);
        } else {
            // Report the error
            spdlog::error("[Resource Manager] Could not load texture! ({})", path);
            success = false;

            // Free any loose data
            stbi_image_free(pixels);
        }
    }

    // Create the texture if successful
    if (success) {

        texture->load(inputTextures, width, height, info);
        _textures.insert(name, texture);
    }

    // Free allocated image resources
    for (auto* tex : inputTextures) {
        stbi_image_free(tex);
    }
}

void ResourceManager::loadModel(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading model '" + name + "'...");

    if (_models.find(name) == _models.end()) {
        auto* model = new Model(path.c_str());
        model->build();

        _models.insert(name, model);
    } else {
        spdlog::error("[Resource Manager] Could not model shader, a model of this name already exists.");
    }
}

Shader* ResourceManager::getShader(std::string name) {
    auto shaderPair = _shaders.find(name);
    if (shaderPair == _shaders.end()) {
        spdlog::error("[Resource Manager] Shader of name {} does not exist! Returning null pointer...", name);
        return nullptr;
    }



    return shaderPair->second;
}

Texture2D* ResourceManager::getTexture(std::string name) {
    auto texturePair = _textures.find(name);
    if (texturePair == _textures.end()) {
        spdlog::error("[Resource Manager] Texture of name {} does not exist! Returning null pointer...", name);
        return nullptr;
    }

    return texturePair->second;
}

Model *ResourceManager::getModel(std::string name) {
    auto modelPair = _models.find(name);
    if (modelPair == _models.end()) {
        spdlog::error("[Resource Manager] Model of name {} does not exist! Returning null pointer...", name);
        return nullptr;
    }

    return modelPair->second;
}

void ResourceManager::cleanup() {
    _shaders.release();
    _shaders.clear();

    _textures.release();
    _textures.clear();

    _models.release();
    _models.clear();
}

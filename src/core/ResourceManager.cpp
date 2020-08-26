#include "ResourceManager.h"

// Instantiate static variables
std::map<std::string, Shader*> ResourceManager::_shaders;
std::map<std::string, Texture2D*> ResourceManager::_textures;

void ResourceManager::loadShader(std::string name, std::string path) {
    //spdlog::info("[Resource Manager] Loading shader '" + name + "'...");
    //_shaders[name] = Shader(std::string(path + ".vert").c_str(), std::string(path + ".frag").c_str());
}

void ResourceManager::loadTexture(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading texture '" + name + "'...");

    // The texture the image will be stored in
    auto* texture = new Texture2D();

    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

    // If successful
    if (data) {
        // Load in the texture
        texture->load(data, width, height);
        _textures[name] = texture;
    } else {
        spdlog::error("[Resource Manager] Could not load texture!");
        delete texture;
    }

    // Free image resources
    stbi_image_free(data);
}

Shader* ResourceManager::getShader(std::string name) {
    return _shaders[name];
}

Texture2D* ResourceManager::getTexture(std::string name) {
    return _textures[name];
}

void ResourceManager::clean() {

}
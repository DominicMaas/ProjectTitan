#include "ResourceManager.h"

// Instantiate static variables
//std::map<std::string, Shader> ResourceManager::_shaders;
std::map<std::string, Texture2D> ResourceManager::_textures;

void ResourceManager::loadShader(std::string name, std::string path) {
    //spdlog::info("[Resource Manager] Loading shader '" + name + "'...");
    //_shaders[name] = Shader(std::string(path + ".vert").c_str(), std::string(path + ".frag").c_str());
}

void ResourceManager::loadTexture(std::string name, std::string path) {
    spdlog::info("[Resource Manager] Loading texture '" + name + "'...");

    // Load in the image
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        // Load the texture and add it to the array
        Texture2D texture;
        texture.load(data, width, height);

        _textures[name] = texture;
    } else {
        spdlog::error("[Resource Manager] Could not load texture!");
    }

    // Free resources
    stbi_image_free(data);
}

Shader* ResourceManager::getShader(std::string name) {
    //return &_shaders[name];
    return nullptr;
}

Texture2D* ResourceManager::getTexture(std::string name) {
    return &_textures[name];
}

void ResourceManager::clean() {

}
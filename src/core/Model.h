#pragma once

#include <pch.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderable.h"
#include "../Mesh.h"

class Model: public Renderable {
public:
    Model(std::string path) {
        loadModel(path);
    }

    Model(Mesh m) {
        _meshes.push_back(m);
    }

    void build(RenderableData input) override;
    void render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName) override;
    void update(RenderableData input, long double deltaTime) override;
    void destroy(RenderableData input) override;

private:

    std::vector<Mesh> _meshes;
    std::string _directory;

    std::vector<Texture> _loadedTextures;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
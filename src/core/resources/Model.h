#pragma once

#include <pch.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/concept_check.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "../Mesh.h"

class Model {
public:
    Model(std::string path) {
        loadModel(path);
    }
    ~Model();

    void build();
    void render(vk::CommandBuffer &commandBuffer, const std::string &pipelineName);

private:

    boost::ptr_vector<Mesh> _meshes;
    std::string _directory;

    std::vector<Texture> _loadedTextures;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
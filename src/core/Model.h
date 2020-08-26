#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <constants.h>
#include "../Shader.h"
#include "../Mesh.h"

class Model {
public:
    Model(std::string path) {
        loadModel(path);
    }

    // Builds all the meshes for rendering
    void build();

    void render(Shader &shader);

private:

    std::vector<Mesh> _meshes;
    std::string _directory;

    std::vector<Texture> _loadedTextures;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
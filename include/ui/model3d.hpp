#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include "mesh.hpp"
#include "shader.hpp"

namespace capvision {
namespace ui {

class Model3D {
public:
    explicit Model3D(const char* path);
    void render(Shader& shader);

private:
    std::vector<Mesh> meshes_;
    std::string directory_;
    std::vector<Texture> texturesLoaded_;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, 
                                            aiTextureType type,
                                            const std::string& typeName);
    unsigned int textureFromFile(const char* path, const std::string& directory);
};

} // namespace ui
} // namespace capvision
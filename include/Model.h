#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <iostream>

#include "Mesh.h"

class Model {
 public:
  Model(char *path) { loadModel(path); }
  Model() = default;
  void Draw(GLuint &program);
  void loadModel(std::string path);
  void isInverted();
  bool is_uv_inverted = false;
  std::vector<Mesh> meshes_;
  std::vector<Texture> textures_loaded;

 private:
  std::string directory_;

  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> load_material_textures(aiMaterial *mat,
                                              aiTextureType type,
                                              std::string type_name);
};

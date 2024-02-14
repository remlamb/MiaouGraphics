#include "Model.h"

void Model::Draw(GLuint &program) {
  for (unsigned int i = 0; i < meshes_.size(); i++) {
    meshes_[i].Draw(program);
  }
}

void Model::loadModel(std::string path) {
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
    return;
  }
  directory_ = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(processMesh(mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    // Process vertex positions, normals and texture coordinates.
    Vertex vertex;

    // Don't convert assimp vector to glm::vector directly because Assimp
    // maintains its own data types for vector, matrices, strings etc, and they
    // don't convert that well to glm's data types.
    vertex.position.x = mesh->mVertices[i].x;
    vertex.position.y = mesh->mVertices[i].y;
    vertex.position.z = mesh->mVertices[i].z;

    vertex.normal.x = mesh->mNormals[i].x;
    vertex.normal.y = mesh->mNormals[i].y;
    vertex.normal.z = mesh->mNormals[i].z;

    vertex.tangent.x = mesh->mTangents[i].x;
    vertex.tangent.y = mesh->mTangents[i].y;
    vertex.tangent.z = mesh->mTangents[i].z;

    // If the mesh contains texture coordinates, stores it.
    if (mesh->mTextureCoords[0]) {
      vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;
      vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }
  // Process indices (each faces has a number of indices).
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    // Diffuse maps.
    std::vector<Texture> diffuseMaps = load_material_textures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    // Specular maps.
    std::vector<Texture> specularMaps = load_material_textures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  // return Mesh(vertices, indices, textures);
  return std::move(Mesh(vertices, indices, textures));
}

void Model::isInverted() { is_uv_inverted = true; }

std::vector<Texture> Model::load_material_textures(aiMaterial *mat,
                                                   aiTextureType type,
                                                   std::string type_name) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); j++) {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      std::string_view Tpath = str.C_Str();
      texture.is_uv_inverted = is_uv_inverted;
      texture.TextureFromFile(directory_ + '/' + Tpath.data());
      texture.type = type_name;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture);
    }
  }
  return textures;
}

#pragma once
#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
  glm::vec3 tangent;
};

struct Texture {
  GLuint id = 0;
  unsigned int hdrTexture;
  std::string type;
  std::string path;
  bool is_uv_inverted = true;
  void TextureFromFile(std::string_view file_path);
  void TextureFromFile(std::string_view file_path, bool is_inverted);
  void TextureFromFileRepeat(std::string_view file_path);
  void HDRTextureFromFile(std::string_view file_path);
  void HDRTextureFromFile(std::string_view file_path, bool is_inverted);
  void HDRTextureFromFileRepeat(std::string_view file_path);
  void BindTexture(GLenum textureUnit);
  void BindTextureHDR(GLenum textureUnit);
  void BindTextureInt(int textureUnit);
};

class Mesh {
 public:
  std::vector<Vertex> vertices_;
  std::vector<GLuint> indices_;
  std::vector<Texture> textures_;
  GLuint vao_;

  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
       std::vector<Texture> textures);
  void Draw(GLuint &program);

 private:
  GLuint vbo_, ebo_;
  void setupMesh();
};

#include <GL/glew.h>

#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"
#include "stb_image.h"

namespace gpr5300 {
class HelloLightmaps final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_lightmaps/cube.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_lightmaps/cube.frag";

  GLuint cube_program;

  const std::string_view lightVertexShaderFilePath_ =
      "data/shaders/hello_lightmaps/cube.vert";
  const std::string_view lightFragmentShaderFilePath_ =
      "data/shaders/hello_lightmaps/light.frag";
  GLuint light_program;

  GLuint vao_ = 0;

  GLuint texture1_ = 0;
  const std::string_view texture1FilePath_ = "data/textures/container.png";

  GLuint texture2_ = 0;
  const std::string_view texture2FilePath_ =
      "data/textures/container_specular.png";

  float timer_ = 0;
  GLuint vbo_[3] = {};
  GLuint ebo_ = 0;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  float vertices[72] = {

      0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

      0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
      -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,

      0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

      0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
      0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

      -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,
      -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f,

  };

  unsigned int indices[36] = {
      0,  1,  3,  1,  2,  3,  4,  5,  7,  5,  6,  7,  8,  9,  11, 9,  10, 11,
      12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 20, 21, 23, 21, 22, 23,
  };

  float texCoords[48] = {

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,
  };

  GLuint lightvao_ = 0;
  GLuint lightebo_ = 0;
  glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

  float normals[72] = {
      0.0f,  0.0f,  1.0f,  // Front face
      0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

      0.0f,  0.0f,  -1.0f,  // Back face
      0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f,

      0.0f,  1.0f,  0.0f,  // left face
      0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      0.0f,  -1.0f, 0.0f,  // right face
      0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,

      1.0f,  0.0f,  0.0f,  // bottom face
      1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -1.0f, 0.0f,  0.0f,  // top face
      -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,
  };
};

static GLuint LoadNewTexture(const char* file_path) {
  GLuint texture = 0;
  stbi_set_flip_vertically_on_load(true);
  int width, height, nbrChannels;
  unsigned char* data = stbi_load(file_path, &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n";
    // return texture;
  }

  std::cout << "width : " << width << " height : " << height
            << " Channels : " << nbrChannels << '\n';

  // Load Texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  return texture;
}

void HelloLightmaps::Begin() {
  cube_program =
      Pipeline::CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);

  light_program = Pipeline::CreateProgram(lightVertexShaderFilePath_,
                                          lightFragmentShaderFilePath_);

  texture1_ = LoadNewTexture(texture1FilePath_.data());
  texture2_ = LoadNewTexture(texture2FilePath_.data());

  // TODO Struct Mesh avec vao vbo ebo (tab de vbo ou vbo avec nom unique)
  // TODO Fonction ou on link les vbo au vao -> avec un ordre defini
  glCreateVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // normal attribute
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
  // (void*)0); glEnableVertexAttribArray(1);
  glGenBuffers(1, &vbo_[2]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &vbo_[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glGenVertexArrays(1, &lightvao_);
  glBindVertexArray(lightvao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &lightebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glEnable(GL_DEPTH_TEST);
  view = glm::translate(view, glm::vec3(1.0f, -1.0f, -3.0f));

  projection =
      glm::perspective(glm::radians(100.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  Pipeline::SetInt(cube_program, "material.diffuse", 0);
  Pipeline::SetInt(cube_program, "material.specular", 1);
}

void HelloLightmaps::End() {
  // Unload program/pipeline
  glDeleteProgram(cube_program);
  glDeleteProgram(light_program);
  glDeleteBuffers(2, &vbo_[0]);
  glDeleteBuffers(1, &ebo_);
  // TODO Mettre warning dans le destructeur si on oublie de Delete (Texture,
  // etc..)
  glDeleteTextures(1, &texture1_);
  glDeleteTextures(1, &texture2_);
  glDeleteVertexArrays(1, &vao_);
}

void HelloLightmaps::Update(float dt) {
  // Draw program

  timer_ += dt;
  float radius = 2.0f;
  lightPos = glm::vec3(radius * cos(timer_), 0.0f, radius * sin(timer_));

  glUseProgram(cube_program);
  glBindVertexArray(vao_);

  Pipeline::SetVec3(cube_program, "viewPos", lightPos);
  Pipeline::SetVec3(cube_program, "light.position", lightPos);

  Pipeline::SetVec3(cube_program, "light.ambient", 0.2f, 0.2f, 0.2f);
  Pipeline::SetVec3(cube_program, "light.diffuse", 0.5f, 0.5f, 0.5f);
  Pipeline::SetVec3(cube_program, "light.specular", 1.0f, 1.0f, 1.0f);

  Pipeline::SetFloat(cube_program, "material.shininess", 64.0f);

  glm::mat4 model = glm::mat4(1.0f);

  Pipeline::SetMat4(cube_program, "model", model);
  Pipeline::SetMat4(cube_program, "view", view);
  Pipeline::SetMat4(cube_program, "projection", projection);

  BindTexture(GL_TEXTURE0, texture1_);
  BindTexture(GL_TEXTURE1, texture2_);

  glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

  glUseProgram(light_program);
  glBindVertexArray(lightvao_);
  Pipeline::SetMat4(light_program, "projection", projection);
  Pipeline::SetMat4(light_program, "view", view);
  model = glm::mat4(1.0f);
  model = glm::translate(model, lightPos);
  model = glm::scale(model, glm::vec3(0.2f));

  Pipeline::SetMat4(light_program, "model", model);
  Pipeline::SetVec3(light_program, "lightPos", lightPos);

  glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloLightmaps scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
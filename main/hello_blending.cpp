#include <GL/glew.h>

#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "Camera.h"
#include "engine.h"
#include "file_utility.h"
#include "Mesh.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloBlending final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_blending/texture.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_blending/texture.frag";
  Pipeline cube_pipeline_;

  const std::string_view outlineVertexShaderFilePath_ =
      "data/shaders/hello_blending/outline.vert";
  const std::string_view outlineFragmentShaderFilePath_ =
      "data/shaders/hello_blending/outline.frag";
  Pipeline outline_pipeline_;

  const std::string_view noAlphaVertexShaderFilePath_ =
      "data/shaders/hello_blending/noAlpha.vert";
  const std::string_view noAlphaFragmentShaderFilePath_ =
      "data/shaders/hello_blending/noAlpha.frag";
  Pipeline no_alpha_pipeline_;

  const std::string_view semiAlphaVertexShaderFilePath_ =
      "data/shaders/hello_blending/noAlpha.vert";
  const std::string_view semiAlphaFragmentShaderFilePath_ =
      "data/shaders/hello_blending/noAlpha.frag";
  Pipeline semi_alpha_pipeline_;

  GLuint vao_ = 0;
  //GLuint texture1_ = 0;
  Texture texture1_;
  const std::string_view texture1FilePath_ = "data/textures/deep.png";

  //GLuint texture2_ = 0;
  Texture texture2_;
  const std::string_view texture2FilePath_ = "data/textures/deep2.png";

  //GLuint texture3_ = 0;
  Texture texture3_;
  const std::string_view texture3FilePath_ = "data/textures/stylized_grass.png";

  //GLuint texture4_ = 0;
  Texture texture4_;
  const std::string_view texture4FilePath_ = "data/textures/window.png";

  float timer_ = 0;
  GLuint vbo_[2] = {};
  GLuint ebo_ = 0;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  float vertices[72] = {

      0.5f,  0.5f,  0.5f,

      0.5f,  -0.5f, 0.5f,

      -0.5f, -0.5f, 0.5f,

      -0.5f, 0.5f,  0.5f,

      0.5f,  0.5f,  -0.5f,

      0.5f,  0.5f,  0.5f,

      -0.5f, 0.5f,  0.5f,

      -0.5f, 0.5f,  -0.5f,

      0.5f,  0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f,

      -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f,

      0.5f,  -0.5f, 0.5f,

      -0.5f, -0.5f, 0.5f,

      -0.5f, -0.5f, -0.5f,

      0.5f,  0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f,

      0.5f,  -0.5f, 0.5f,

      0.5f,  0.5f,  0.5f,

      -0.5f, 0.5f,  0.5f,

      -0.5f, -0.5f, 0.5f,

      -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f,

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

  glm::vec3 cubePositions[10] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  Camera camera_;

  GLuint plane_vao_ = 0;
  GLuint plane_vbo_ = 0;
  float transparentVertices[30] = {
      // positions         // texture Coords (swapped y coordinates because
      // texture is flipped upside down)
      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
      0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
      1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

  std::vector<glm::vec3> vegetation{
      glm::vec3(-1.5f, 0.0f, -0.49f), glm::vec3(1.5f, 0.0f, 0.50f),
      glm::vec3(0.0f, 0.0f, 0.69f), glm::vec3(-0.3f, 0.0f, -2.31f),
      glm::vec3(0.5f, 0.0f, -0.61f)};

  std::vector<glm::vec3> window{
      glm::vec3(-1.5f, 0.0f, -0.48f), glm::vec3(1.5f, 0.0f, 0.51f),
      glm::vec3(0.0f, 0.0f, 0.7f), glm::vec3(-0.3f, 0.0f, -2.3f),
      glm::vec3(0.5f, 0.0f, -0.6f)};
};

void HelloBlending::Begin() {
  outline_pipeline_.CreateProgram(outlineVertexShaderFilePath_,
                                  outlineFragmentShaderFilePath_);

  cube_pipeline_.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);

  no_alpha_pipeline_.CreateProgram(noAlphaVertexShaderFilePath_,
                                   noAlphaFragmentShaderFilePath_);

  semi_alpha_pipeline_.CreateProgram(semiAlphaVertexShaderFilePath_,
                                     semiAlphaFragmentShaderFilePath_);

  texture1_.TextureFromFile(texture1FilePath_.data());
  texture2_.TextureFromFile(texture2FilePath_.data());
  texture3_.TextureFromFile(texture3FilePath_.data());
  texture4_.TextureFromFile(texture4FilePath_.data());

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glCreateVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &vbo_[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // plane with Grass
  glGenVertexArrays(1, &plane_vao_);
  glGenBuffers(1, &plane_vbo_);
  glBindVertexArray(plane_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, plane_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices),
               transparentVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glBindVertexArray(0);

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
}

void HelloBlending::End() {
  // Unload program/pipeline
  glDeleteBuffers(2, &vbo_[0]);
  glDeleteBuffers(1, &ebo_);
  glDeleteTextures(1, &texture1_.id);
  glDeleteTextures(1, &texture2_.id);
  glDeleteTextures(1, &texture3_.id);
  glDeleteTextures(1, &texture4_.id);
  glDeleteVertexArrays(1, &vao_);
}

void HelloBlending::Update(float dt) {
  // Draw program
  glClear(GL_STENCIL_BUFFER_BIT);
  glBindVertexArray(vao_);

  timer_ += dt;
  const float value = (std::cos(timer_ * 1.5f) + 1.0f) / 2.0f;
  cube_pipeline_.SetFloat("value", value);

  camera_.Update(dt);
  view = camera_.view_;

  for (unsigned int i = 0; i < 10; i++) {
    // 1st part : draw objects as normal, writing to the stencil
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);

    texture1_.BindTexture(GL_TEXTURE0);
    cube_pipeline_.SetInt("tex1", 0);

    texture2_.BindTexture(GL_TEXTURE1);
    cube_pipeline_.SetInt("tex2", 1);

    cube_pipeline_.SetMat4("model", model);
    cube_pipeline_.SetMat4("view", view);
    cube_pipeline_.SetMat4("projection", projection);

    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

    //// 2nd Part : Outline Cube
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);

    float scale = 1.2f;
    model = glm::scale(model, glm::vec3(scale, scale, scale));

    outline_pipeline_.SetMat4("model", model);
    outline_pipeline_.SetMat4("view", view);
    outline_pipeline_.SetMat4("projection", projection);

    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
  }

  // vegetation
  glBindVertexArray(plane_vao_);
  texture3_.BindTexture(GL_TEXTURE2);

  for (unsigned int i = 0; i < vegetation.size(); i++) {
    no_alpha_pipeline_.SetInt("texture1", 2);
    model = glm::mat4(1.0f);
    model = glm::translate(model, vegetation[i]);
    model =
        glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    no_alpha_pipeline_.SetMat4("model", model);
    no_alpha_pipeline_.SetMat4("view", view);
    no_alpha_pipeline_.SetMat4("projection", projection);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  // Window
  glBindVertexArray(plane_vao_);
  texture4_.BindTexture(GL_TEXTURE3);

  std::map<float, glm::vec3> sorted;
  for (unsigned int i = 0; i < window.size(); i++) {
    float distance = glm::length(camera_.position_ - window[i]);
    sorted[distance] = window[i];
  }

  for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
    semi_alpha_pipeline_.SetInt("texture1", 3);
    model = glm::mat4(1.0f);
    model = glm::translate(model, it->second);
    model =
        glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    semi_alpha_pipeline_.SetMat4("model", model);
    semi_alpha_pipeline_.SetMat4("view", view);
    semi_alpha_pipeline_.SetMat4("projection", projection);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloBlending scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
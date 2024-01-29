#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "Model.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloModel final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_model/model.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_model/model.frag";

  Pipeline model_pipeline;

  const std::string_view model_path = "data/model/backpack/backpack.obj";
  Model ourModel_;

  const std::string_view cat_model_path = "data/model/cat/newcat.obj";
  Model cat_;

  const std::string_view grassVertexShaderFilePath_ =
      "data/shaders/hello_model/moving_grass.vert";
  const std::string_view grassFragmentShaderFilePath_ =
      "data/shaders/hello_model/moving_grass.frag";
  Pipeline grass_pipeline;

  const std::string_view michelle_model_path =
      "data/model/Michelle/Michelle.obj";
  Model michelle_;

  Camera camera_;

  const std::string_view fern_model_path = "data/model/Fern/fern_new.obj";
  Model fern_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  float timer_ = 0.0f;

  GLuint plane_vao_ = 0;
  GLuint plane_vbo_ = 0;
  float transparentVertices[30] = {
      // positions         // texture Coords (swapped y coordinates because
      // texture is flipped upside down)
      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
      0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
      1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

  Texture grass_texture_;
  const std::string_view grass_texture_FilePath_ =
      "data/textures/stylized_grass.png";

  const std::string_view grass2dVertexShaderFilePath_ =
      "data/shaders/hello_model/moving_grass2d.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_model/moving_grass2d.frag";
  Pipeline grass2d_pipeline;
};

void HelloModel::Begin() {
  model_pipeline.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);

  grass_pipeline.CreateProgram(grassVertexShaderFilePath_,
                               grassFragmentShaderFilePath_);

  grass2d_pipeline.CreateProgram(grass2dVertexShaderFilePath_,
                                 grass2dFragmentShaderFilePath_);

  grass_texture_.is_uv_inverted = false;
  grass_texture_.TextureFromFile(grass_texture_FilePath_);
  grass_texture_.BindTexture(GL_TEXTURE0);

  ourModel_.isInverted();
  ourModel_.loadModel(model_path.data());

  michelle_.loadModel(michelle_model_path.data());

  cat_.loadModel(cat_model_path.data());

  fern_.loadModel(fern_model_path.data());

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  glEnable(GL_DEPTH_TEST);

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
}

void HelloModel::End() {
  // Unload program/pipeline
}

void HelloModel::Update(float dt) {
  // Draw program
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  grass_texture_.BindTexture(GL_TEXTURE0);
  glBindVertexArray(plane_vao_);
  float dimtime = 10 * timer_;
  float dimwindStrength = 0.2f;  // Adjust as needed
  grass2d_pipeline.SetFloat("time", dimtime);
  grass2d_pipeline.SetFloat("windStrength", dimwindStrength);
  grass2d_pipeline.SetInt("texture_diffuse1", 9);

  model = glm::scale(model, glm::vec3(1, 1, 1));
  grass2d_pipeline.SetMat4("model", model);
  grass2d_pipeline.SetMat4("view", view);
  grass2d_pipeline.SetMat4("projection", projection);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
  model_pipeline.SetMat4("model", model);
  model_pipeline.SetMat4("view", view);
  model_pipeline.SetMat4("projection", projection);
  grass2d_pipeline.SetInt("texture_diffuse1", 9);

  ourModel_.Draw(model_pipeline.program_);

  float scale = 8.2f;
  model = glm::scale(model, glm::vec3(scale, scale, scale));
  model = glm::translate(model, glm::vec3(0.6f, -0.3f, -0.3f));
  model_pipeline.SetMat4("model", model);
  model_pipeline.SetMat4("view", view);
  model_pipeline.SetMat4("projection", projection);

  cat_.Draw(model_pipeline.program_);
  michelle_.Draw(model_pipeline.program_);

  float time = 100 * timer_;
  float windStrength = 0.02f;  // Adjust as needed
  grass_pipeline.SetFloat("time", time);
  grass_pipeline.SetFloat("windStrength", windStrength);
  model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
  model = glm::translate(model, glm::vec3(0.6f, 0.0f, 0.0f));
  grass_pipeline.SetMat4("model", model);
  grass_pipeline.SetMat4("view", view);
  grass_pipeline.SetMat4("projection", projection);
  fern_.Draw(grass_pipeline.program_);
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloModel scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
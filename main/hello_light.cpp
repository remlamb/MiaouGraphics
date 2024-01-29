#include <GL/glew.h>

#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "Mesh.h"
#include "PrimitiveObjects.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloCube final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void LoadTexture(GLuint& texture, const char* file_path);
  void CreateProgram(std::string_view vertex_shader_file_path,
                     std::string_view fragment_shader_file_path);

 private:
  float timer_ = 0;

  const std::string_view cubeVertexShaderFilePath_ =
      "data/shaders/hello_light/cube.vert";
  const std::string_view cubeFragmentShaderFilePath_ =
      "data/shaders/hello_light/cube.frag";
  Pipeline cube_pipeline_;
  const std::string_view lightFragmentShaderFilePath_ =
      "data/shaders/hello_light/light.frag";
  Pipeline light_pipeline_;

  PrimitiveObjects cube_;
  PrimitiveObjects light_cube_;
  Camera camera_;

  Texture texture_;
  const std::string_view texture_FilePath_ = "data/textures/deep2.png";

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
};

void HelloCube::Begin() {
  cube_pipeline_.CreateProgram(cubeVertexShaderFilePath_,
                               cubeFragmentShaderFilePath_);

  light_pipeline_.CreateProgram(cubeVertexShaderFilePath_,
                                lightFragmentShaderFilePath_);

  texture_.is_uv_inverted = false;
  texture_.TextureFromFile(texture_FilePath_);
  texture_.BindTexture(GL_TEXTURE0);

  cube_.SetUpCube();
  light_cube_.SetUpCube();

  glEnable(GL_DEPTH_TEST);
  view = glm::translate(view, glm::vec3(1.0f, -1.0f, -3.0f));

  projection =
      glm::perspective(glm::radians(100.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
}

void HelloCube::End() {
  // Unload program/pipeline
  // TODO Mettre warning dans le destructeur si on oublie de Delete Texture
  // Peut etre fait pour les autres elements aussi
}

void HelloCube::Update(float dt) {
  // Draw program
  camera_.Update(dt);
  view = camera_.view_;
  timer_ += dt;

  float radius = 2.0f;
  lightPos.x = +radius * cos(timer_);
  lightPos.y = 0.0f;
  lightPos.z = +radius * sin(timer_);

  glm::mat4 model = glm::mat4(1.0f);
  cube_pipeline_.SetMat4("model", model);
  cube_pipeline_.SetMat4("view", view);
  cube_pipeline_.SetMat4("projection", projection);

  cube_pipeline_.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
  cube_pipeline_.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
  cube_pipeline_.SetVec3("lightPos", lightPos);
  cube_.RenderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, lightPos);
  model = glm::scale(model, glm::vec3(0.2f));
  light_pipeline_.SetMat4("model", model);
  light_pipeline_.SetMat4("view", view);
  light_pipeline_.SetMat4("projection", projection);
  light_pipeline_.SetVec3("lightPos", lightPos);
  light_cube_.RenderCube();
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloCube scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
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
class HelloLightCaster final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view cubeVertexShaderFilePath_ =
      "data/shaders/hello_light_caster/point_light.vert";
  const std::string_view cubeFragmentShaderFilePath_ =
      "data/shaders/hello_light_caster/point_light.frag";
  Pipeline cube_pipeline_;
  const std::string_view pointlightVertexShaderFilePath_ =
      "data/shaders/hello_light_caster/point_light.vert";
  const std::string_view pointlightFragmentShaderFilePath_ =
      "data/shaders/hello_light_caster/point_light.frag";
  Pipeline light_pipeline_;

  PrimitiveObjects cube_;

  Texture texture_;
  const std::string_view texture_FilePath_ = "data/textures/deep2.png";

  Texture textureDiffuse_;
  const std::string_view textureDiffuse_FilePath_ =
      "data/textures/container.png";

  Texture textureSpecular_;
  const std::string_view textureSpecular_FilePath_ =
      "data/textures/container_specular.png";

  float timer_ = 0;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  glm::vec3 cubePositions[10] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  PrimitiveObjects lightCube_;
  glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

  Camera camera_;
};

void HelloLightCaster::Begin() {
  cube_pipeline_.CreateProgram(cubeVertexShaderFilePath_,
                               cubeFragmentShaderFilePath_);

  light_pipeline_.CreateProgram(pointlightVertexShaderFilePath_,
                                pointlightFragmentShaderFilePath_);

  textureDiffuse_.is_uv_inverted = true;
  textureDiffuse_.TextureFromFile(textureDiffuse_FilePath_);
  textureDiffuse_.BindTexture(GL_TEXTURE1);

  textureSpecular_.is_uv_inverted = false;
  textureSpecular_.TextureFromFile(textureSpecular_FilePath_);
  textureSpecular_.BindTexture(GL_TEXTURE2);

  cube_.SetUpCubeOpenGL();
  lightCube_.SetUpCubeOpenGL();

  glEnable(GL_DEPTH_TEST);
  view = glm::translate(view, glm::vec3(1.0f, -1.0f, -3.0f));

  projection =
      glm::perspective(glm::radians(100.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  cube_pipeline_.SetInt("material.diffuse", 1);
  cube_pipeline_.SetInt("material.specular", 2);
}

void HelloLightCaster::End() {
  // Unload program/pipeline

  // TODO Mettre warning dans le destructeur si on oublie de Delete (Texture,
  // etc..)
}

void HelloLightCaster::Update(float dt) {
  // Draw program
  camera_.Update(dt);
  view = camera_.view_;

  timer_ += dt;
  float radius = 2.0f;
  lightPos = glm::vec3(radius * cos(timer_), 0.0f, radius * sin(timer_));

  model = glm::mat4(1.0f);
  model = glm::translate(model, lightPos);
  model = glm::scale(model, glm::vec3(0.2f));

  light_pipeline_.SetMat4("model", model);
  light_pipeline_.SetMat4("view", view);
  light_pipeline_.SetMat4("projection", projection);
  light_pipeline_.SetVec3("viewPos", camera_.position_);

  // color
  light_pipeline_.SetVec3("light.ambient", 0.2f, 0.3f, 0.7f);
  light_pipeline_.SetVec3("light.diffuse", 0.2f, 0.3f, 0.7f);
  light_pipeline_.SetVec3("light.specular", 0.2f, 0.3f, 0.7f);

  light_pipeline_.SetFloat("material.shininess", 64.0f);

  light_pipeline_.SetFloat("light.constant", 1.0f);
  light_pipeline_.SetFloat("light.linear", 0.09f);
  light_pipeline_.SetFloat("light.quadratic", 0.032f);
  light_pipeline_.SetVec3("light.position", lightPos);

  lightCube_.RenderCubeOpenGL();

  cube_pipeline_.SetVec3("viewPos", lightPos);
  cube_pipeline_.SetVec3("light.direction", 0.0f, 0.0f, -1.0f);
  cube_pipeline_.SetFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

  cube_pipeline_.SetVec3("light.position", lightPos);

  cube_pipeline_.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
  cube_pipeline_.SetVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
  cube_pipeline_.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);

  cube_pipeline_.SetVec3("light.diffuse", 0.2f, 0.3f, 0.7f);
  cube_pipeline_.SetVec3("light.specular", 0.2f, 0.3f, 0.7f);

  textureDiffuse_.BindTexture(GL_TEXTURE1);
  textureSpecular_.BindTexture(GL_TEXTURE2);
  // cube_pipeline_.SetInt("material.diffuse", 1);
  cube_pipeline_.SetInt("material.diffuse", 1);
  cube_pipeline_.SetInt("material.specular", 2);

  cube_pipeline_.SetFloat("material.shininess", 64.0f);

  cube_pipeline_.SetFloat("light.constant", 1.0f);
  cube_pipeline_.SetFloat("light.linear", 0.09f);
  cube_pipeline_.SetFloat("light.quadratic", 0.032f);

  for (unsigned int i = 0; i < 10; i++) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);
    model = glm::scale(model, glm::vec3(0.4f));

    float angle = 80 + 20.0f * i;
    model =
        glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    cube_pipeline_.SetMat4("model", model);
    cube_pipeline_.SetMat4("view", view);
    cube_pipeline_.SetMat4("projection", projection);

    cube_.RenderCubeOpenGL();
  }
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloLightCaster scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
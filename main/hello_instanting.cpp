#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "Cubemaps.h"
#include "Model.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloInstanting final : public Scene {
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

  const std::string_view instantiate_vertexShaderFilePath_ =
      "data/shaders/hello_instantiate/instantiate.vert";
  const std::string_view instantiate_fragmentShaderFilePath_ =
      "data/shaders/hello_instantiate/instantiate.frag";

  Pipeline instantiate_pipeline;

  const std::string_view planet_model_path = "data/model/Planet/planet.obj";
  Model planetModel_;

  Camera camera_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  // must be pow2 number
  unsigned int amount = 40000;

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
      "data/shaders/hello_instantiate/moving_grass2d.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_instantiate/moving_grass2d.frag";
  Pipeline grass2d_pipeline;

  float timer_ = 0.0f;

  Cubemaps cubemaps_;

  std::vector<std::string> faces{"data/textures/xiequ_yuan/right.png",
                                 "data/textures/xiequ_yuan/left.png",
                                 "data/textures/xiequ_yuan/top.png",
                                 "data/textures/xiequ_yuan/bottom.png",
                                 "data/textures/xiequ_yuan/front.png",
                                 "data/textures/xiequ_yuan/back.png"};

  const std::string_view cubemapsVertexShaderFilePath_ =
      "data/shaders/hello_cubemaps/cubemaps.vert";
  const std::string_view cubemapsFragmentShaderFilePath_ =
      "data/shaders/hello_cubemaps/cubemaps.frag";

  Pipeline cubemaps_pipeline;
};

void HelloInstanting::Begin() {
  model_pipeline.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);
  instantiate_pipeline.CreateProgram(instantiate_vertexShaderFilePath_,
                                     instantiate_fragmentShaderFilePath_);

  planetModel_.loadModel(planet_model_path.data());

  cubemaps_.SetUp(faces);
  cubemaps_pipeline.CreateProgram(cubemapsVertexShaderFilePath_,
                                  cubemapsFragmentShaderFilePath_);
  cubemaps_pipeline.SetInt("skybox", 0);

  grass2d_pipeline.CreateProgram(grass2dVertexShaderFilePath_,
                                 grass2dFragmentShaderFilePath_);

  grass_texture_.is_uv_inverted = false;
  grass_texture_.TextureFromFile(grass_texture_FilePath_);
  grass_texture_.BindTexture(GL_TEXTURE10);


  // Plane Grass2DMesh
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

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  // generate a large list of semi-random model transformation matrices
  // ------------------------------------------------------------------
  // glm::mat4* modelMatrices;
  // modelMatrices = new glm::mat4[amount];
  // float radius = 100.0;
  // float offset = 25.0f;
  // for (unsigned int i = 0; i < amount; i++) {
  //  glm::mat4 model = glm::mat4(1.0f);
  //  // 1. translation: displace along circle with 'radius' in range [-offset,
  //  // offset]
  //  float angle = (float)i / (float)amount * 360.0f;
  //  float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
  //  float x = sin(angle) * radius + displacement;
  //  displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
  //  float y = displacement * 0.4f;  // keep height of asteroid field smaller
  //                                  // compared to width of x and z
  //  displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
  //  float z = cos(angle) * radius + displacement;
  //  model = glm::translate(model, glm::vec3(x, y, z));

  //  // 2. scale: Scale between 0.05 and 0.25f
  //  float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
  //  model = glm::scale(model, glm::vec3(scale));

  //  // 3. rotation: add random rotation around a (semi)randomly picked
  //  rotation
  //  // axis vector
  //  float rotAngle = static_cast<float>((rand() % 360));
  //  model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

  //  // 4. now add to list of matrices
  //  modelMatrices[i] = model;

  // Position Pour L'herbe
  glm::mat4* modelMatrices;
  modelMatrices = new glm::mat4[3 * amount];
  float separation = 0.12f;  // Adjust the separation between objects
  float amout_squareroot = sqrt(amount);
  float pos = 10;
  int i = 0;
  int rotationPreset = 0;
  for (unsigned int zIndex = 0; zIndex < amout_squareroot; zIndex++) {
    for (unsigned int xIndex = 0; xIndex < amout_squareroot; xIndex++) {
      glm::mat4 model = glm::mat4(1.0f);
      float z = static_cast<float>(zIndex) * separation;
      float x = -10.0f + static_cast<float>(xIndex) * separation;
      float y = -1.0f;  // or add randomness if needed
      model = glm::translate(model, glm::vec3(x, y, z));

      // TODO better random Scaling
      float scale = static_cast<float>((rand() % 20) / 50.0 + 0.05);
      model = glm::scale(model, glm::vec3(scale));

      // Rotation
      modelMatrices[i] = model;
      i++;

      model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrices[i] = model;
      i++;

      model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrices[i] = model;
      i++;
    }
  }

  // configure instanced array
  // -------------------------
  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0],
               GL_STATIC_DRAW);

  // set transformation matrices as an instance vertex attribute (with divisor
  // 1) note: we're cheating a little by taking the, now publicly declared, VAO
  // of the model's mesh(es) and adding new vertexAttribPointers normally you'd
  // want to do this in a more organized fashion, but for learning purposes this
  // will do.
  // -----------------------------------------------------------------------------------------------------------------------------------
  // for (unsigned int i = 0; i < planetModel_.meshes_.size(); i++) {
  //  unsigned int VAO = planetModel_.meshes_[i].vao_;
  //  glBindVertexArray(plane_vao_);
  //  // set attribute pointers for matrix (4 times vec4)
  //  glEnableVertexAttribArray(3);
  //  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
  //                        (void*)0);
  //  glEnableVertexAttribArray(4);
  //  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
  //                        (void*)(sizeof(glm::vec4)));
  //  glEnableVertexAttribArray(5);
  //  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
  //                        (void*)(2 * sizeof(glm::vec4)));
  //  glEnableVertexAttribArray(6);
  //  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
  //                        (void*)(3 * sizeof(glm::vec4)));

  //  glVertexAttribDivisor(3, 1);
  //  glVertexAttribDivisor(4, 1);
  //  glVertexAttribDivisor(5, 1);
  //  glVertexAttribDivisor(6, 1);

  //  glBindVertexArray(0);
  //}

  glBindVertexArray(plane_vao_);
  // set attribute pointers for matrix (4 times vec4)
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(3 * sizeof(glm::vec4)));

  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);

  //// grass
  // glGenBuffers(1, &plane_vbo_);
  // glBindBuffer(GL_ARRAY_BUFFER, plane_vbo_);
  // glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4),
  // &modelMatrices[0],
  //              GL_STATIC_DRAW);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);

  // glGenVertexArrays(1, &plane_vao_);
  // glGenBuffers(1, &plane_vbo_);
  // glBindVertexArray(plane_vbo_);
  // glBindBuffer(GL_ARRAY_BUFFER, plane_vbo_);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices),
  //              transparentVertices, GL_STATIC_DRAW);
  // glEnableVertexAttribArray(0);
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
  // (void*)0); glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2,
  // GL_FLOAT, GL_FALSE, 5 * sizeof(float),
  //                       (void*)(3 * sizeof(float)));
  //// also set instance data
  // glEnableVertexAttribArray(2);
  // glBindBuffer(
  //     GL_ARRAY_BUFFER,
  //     plane_vbo_);  // this attribute comes from a different vertex buffer
  // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
  // (void*)0); glBindBuffer(GL_ARRAY_BUFFER, 0); glVertexAttribDivisor(2, 1);

  glEnable(GL_DEPTH_TEST);
}

void HelloInstanting::End() {}

void HelloInstanting::Update(float dt) {
  camera_.Update(dt);
  view = camera_.view_;

  model = glm::mat4(1.0f);
  timer_ += dt;

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.5f));
  model_pipeline.SetMat4("model", model);
  model_pipeline.SetMat4("view", view);
  model_pipeline.SetMat4("projection", projection);
  model_pipeline.SetVec3("cameraPos", camera_.position_);
  planetModel_.Draw(model_pipeline.program_);

  // instantiate_pipeline.SetMat4("view", view);
  // instantiate_pipeline.SetMat4("projection", projection);
  // instantiate_pipeline.SetInt("texture_diffuse1", 0);
  // for (unsigned int i = 0; i < planetModel_.meshes_.size(); i++) {
  //   glBindVertexArray(planetModel_.meshes_[i].vao_);
  //   glDrawElementsInstanced(
  //       GL_TRIANGLES,
  //       static_cast<unsigned int>(planetModel_.meshes_[i].indices_.size()),
  //       GL_UNSIGNED_INT, 0, amount);
  // }

  float dimtime = 2 * timer_;
  float dimwindStrength = 0.18f;  // Adjust as needed
  grass2d_pipeline.SetFloat("time", dimtime);
  grass2d_pipeline.SetFloat("windStrength", dimwindStrength);

  grass_texture_.BindTexture(GL_TEXTURE10);
  grass2d_pipeline.SetInt("texture1", 10);

  // for (unsigned int i = 0; i < planetModel_.meshes_.size(); i++) {

  //  glBindVertexArray(planetModel_.meshes_[i].vao_);
  //  glDrawElementsInstanced(
  //      GL_TRIANGLES,
  //      static_cast<unsigned int>(planetModel_.meshes_[i].indices_.size()),
  //      GL_UNSIGNED_INT, 0, amount);
  //}
  model = glm::scale(model, glm::vec3(1, 1, 1));
  grass2d_pipeline.SetMat4("model", model);
  grass2d_pipeline.SetMat4("view", view);
  grass2d_pipeline.SetMat4("projection", projection);
  glBindVertexArray(plane_vao_);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6,
                        amount);  // 100 triangles of 6 vertices each
  glBindVertexArray(0);
  // planetModel_.Draw(model_pipeline.program_);

  view = glm::mat4(
      glm::mat3(camera_.view_));  // remove translation from the view matrix
  cubemaps_pipeline.SetMat4("view", view);
  cubemaps_pipeline.SetMat4("projection", projection);
  cubemaps_.Draw();
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloInstanting scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
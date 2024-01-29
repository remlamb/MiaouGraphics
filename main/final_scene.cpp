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
#include "FrameBuffer.h"
#include "InstantiateGrass.h"
#include "Model.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class FinalScene final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_framebuffer/model.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_framebuffer/model.frag";

  Pipeline model_pipeline;

  const std::string_view model_path = "data/model/backpack/backpack.obj";
  Model ourModel_;

  const std::string_view cat_model_path = "data/model/cat/newcat.obj";
  Model cat_;

  const std::string_view lantern_model_path = "data/model/lantern/lantern.obj";
  Model lantern_;

  const std::string_view grassVertexShaderFilePath_ =
      "data/shaders/hello_model/moving_grass.vert";
  const std::string_view grassFragmentShaderFilePath_ =
      "data/shaders/hello_model/moving_grass.frag";
  Pipeline grass_pipeline;

  Camera camera_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  float timer_ = 0.0f;

  Texture grass_texture_;
  const std::string_view grass_texture_FilePath_ =
      "data/textures/stylized_grass.png";

  const std::string_view grass2dVertexShaderFilePath_ =
      "data/shaders/hello_instantiate/moving_grass2d.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_instantiate/moving_grass2d.frag";
  Pipeline grass2d_pipeline;

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

  const std::string_view screenVertexShaderFilePath_ =
      "data/shaders/hello_framebuffer/framebuffer.vert";
  const std::string_view screenFragmentShaderFilePath_ =
      "data/shaders/hello_framebuffer/framebuffer.frag";

  Pipeline screen_pipeline;
  FrameBuffer framebuffer_;

  InstantiateGrass grass_;
};

void FinalScene::Begin() {
  cubemaps_.SetUp(faces);
  cubemaps_pipeline.CreateProgram(cubemapsVertexShaderFilePath_,
                                  cubemapsFragmentShaderFilePath_);
  cubemaps_pipeline.SetInt("skybox", 0);

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

  cat_.loadModel(cat_model_path.data());

  lantern_.loadModel(lantern_model_path.data());

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  glEnable(GL_DEPTH_TEST);
  grass_.SetUp();

  framebuffer_.SetUp(Engine::screen_width_, Engine::screen_height_);
  screen_pipeline.CreateProgram(screenVertexShaderFilePath_,
                                screenFragmentShaderFilePath_);
}

void FinalScene::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
  framebuffer_.Delete();
}

void FinalScene::Update(float dt) {
  framebuffer_.Reset();
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
  model_pipeline.SetMat4("model", model);
  model_pipeline.SetMat4("view", view);
  model_pipeline.SetMat4("projection", projection);
  model_pipeline.SetVec3("cameraPos", camera_.position_);
  model_pipeline.SetInt("textureMode", cubemaps_.texture_mode_);
  ourModel_.Draw(model_pipeline.program_);

  float scale = 8.2f;
  model = glm::scale(model, glm::vec3(scale, scale, scale));
  model = glm::translate(model, glm::vec3(0.6f, -0.3f, -0.3f));
  model_pipeline.SetMat4("model", model);
  model_pipeline.SetMat4("view", view);
  model_pipeline.SetMat4("projection", projection);
  model_pipeline.SetVec3("cameraPos", camera_.position_);
  model_pipeline.SetInt("textureMode", cubemaps_.texture_mode_);
  cat_.Draw(model_pipeline.program_);


  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.6f, -0.3f, -0.3f));
  model_pipeline.SetMat4("model", model);
  lantern_.Draw(model_pipeline.program_);

  float dimtime = 2 * timer_;
  float dimwindStrength = 0.18f;  // Adjust as needed
  grass2d_pipeline.SetFloat("time", dimtime);
  grass2d_pipeline.SetFloat("windStrength", dimwindStrength);
  grass_texture_.BindTexture(GL_TEXTURE10);
  grass2d_pipeline.SetInt("texture1", 10);
  model = glm::scale(model, glm::vec3(1, 1, 1));
  grass2d_pipeline.SetMat4("model", model);
  grass2d_pipeline.SetMat4("view", view);
  grass2d_pipeline.SetMat4("projection", projection);
  grass_.Render();

  view = glm::mat4(
      glm::mat3(camera_.view_));  // remove translation from the view matrix
  cubemaps_pipeline.SetMat4("view", view);
  cubemaps_pipeline.SetMat4("projection", projection);
  cubemaps_.Draw();

  framebuffer_.Clear();
  screen_pipeline.SetInt("PostProcessingType", framebuffer_.post_process_type_);
  screen_pipeline.SetInt("screenTexture", 0);
  framebuffer_.Draw();
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::FinalScene scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
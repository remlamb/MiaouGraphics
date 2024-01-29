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
#include "Model.h"
#include "PrimitiveObjects.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloHDRCubemap final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view textVertexShaderFilePath_ =
      "data/shaders/hello_HDRCubemap/pbr_text.vert";
  const std::string_view textFragmentShaderFilePath_ =
      "data/shaders/hello_HDRCubemap/pbr_text.frag";

  Pipeline pipeline;

  Texture albedo;
  const std::string_view albedoFilePath_ =
      "data/textures/rust/rust_basecolor.png";

  Texture normal;
  const std::string_view normalFilePath_ = "data/textures/rust/rust_normal.png";

  Texture metallic;
  const std::string_view metallicFilePath_ =
      "data/textures/rust/rust_metallic.png";

  Texture roughness;
  const std::string_view roughnessFilePath_ =
      "data/textures/rust/rust_roughness.png";

  Texture ao;
  const std::string_view aoFilePath_ = "data/textures/rust/rust_ao.png";

  Camera camera_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

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

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  glm::vec3 lightPositions[4] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };
  glm::vec3 lightColors[4] = {
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

  int nrRows = 7;
  int nrColumns = 7;
  float spacing = 2.5;

  const std::string_view equirectangleVertexShaderFilePath_ =
      "data/shaders/hello_HDRCubemap/cubemap_hdr.vert";
  const std::string_view equirectangleFragmentShaderFilePath_ =
      "data/shaders/hello_HDRCubemap/cubemap_hdr.frag";

  Pipeline equirectangular_to_cubemap_pipeline;

  const std::string_view backgroundVertexShaderFilePath_ =
      "data/shaders/hello_HDRCubemap/background.vert";
  const std::string_view backgroundFragmentShaderFilePath_ =
      "data/shaders/hello_HDRCubemap/background.frag";

  Pipeline backgroundShader;

  unsigned int captureFBO;
  unsigned int captureRBO;

  Texture hdr_cubemap;
  const std::string_view hdr_cubemapFilePath_ =
      "data/textures/xiequ_yuan_hdr/xiequ_yuan.hdr";

  // const std::string_view hdr_cubemapFilePath_ =
  //    "data/textures/newport_hdr/newport.hdr";

  //unsigned int envCubemap;
  PrimitiveObjects sphere_;
  Cubemaps cubemapsHDR_;
};

void HelloHDRCubemap::Begin() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  sphere_.SetUpSphere();
  pipeline.CreateProgram(textVertexShaderFilePath_,
                         textFragmentShaderFilePath_);

  equirectangular_to_cubemap_pipeline.CreateProgram(
      equirectangleVertexShaderFilePath_, equirectangleFragmentShaderFilePath_);

  backgroundShader.CreateProgram(backgroundVertexShaderFilePath_,
                                 backgroundFragmentShaderFilePath_);

  projection = glm::perspective(glm::radians(50.0f),
                                static_cast<float>(Engine::screen_width_) /
                                    static_cast<float>(Engine::screen_height_),
                                0.1f, 100.0f);
  pipeline.SetMat4("projection", projection);

  backgroundShader.SetInt("environmentMap", 0);

  hdr_cubemap.HDRTextureFromFile(hdr_cubemapFilePath_.data());

  cubemapsHDR_.SetUpHDR(equirectangular_to_cubemap_pipeline, hdr_cubemap);

  // initialize static shader uniforms before rendering
  // --------------------------------------------------

  projection = glm::perspective(glm::radians(50.0f),
                                static_cast<float>(Engine::screen_width_) /
                                    static_cast<float>(Engine::screen_height_),
                                0.1f, 100.0f);

  pipeline.SetMat4("projection", projection);
  backgroundShader.SetMat4("projection", projection);

  albedo.TextureFromFile(albedoFilePath_.data());
  normal.TextureFromFile(normalFilePath_.data());
  metallic.TextureFromFile(metallicFilePath_.data());
  roughness.TextureFromFile(roughnessFilePath_.data());
  ao.TextureFromFile(aoFilePath_.data());

  albedo.BindTexture(GL_TEXTURE0);
  normal.BindTexture(GL_TEXTURE1);
  metallic.BindTexture(GL_TEXTURE2);
  roughness.BindTexture(GL_TEXTURE3);
  ao.BindTexture(GL_TEXTURE4);

  pipeline.SetInt("albedoMap", 0);
  pipeline.SetInt("normalMap", 1);
  pipeline.SetInt("metallicMap", 2);
  pipeline.SetInt("roughnessMap", 3);
  pipeline.SetInt("aoMap", 4);

  backgroundShader.SetInt("environmentMap", 0);

  glViewport(0, 0, 1280, 720);
}

void HelloHDRCubemap::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

void HelloHDRCubemap::Update(float dt) {
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  // render
  // ------
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  pipeline.SetMat4("view", view);
  pipeline.SetVec3("camPos", camera_.position_);

  // render rows*column number of spheres with material properties defined by
  // textures (they all have the same material properties)
  albedo.BindTexture(GL_TEXTURE0);
  normal.BindTexture(GL_TEXTURE1);
  metallic.BindTexture(GL_TEXTURE2);
  roughness.BindTexture(GL_TEXTURE3);
  ao.BindTexture(GL_TEXTURE4);

  glm::mat4 model = glm::mat4(1.0f);
  for (int row = 0; row < nrRows; ++row) {
    for (int col = 0; col < nrColumns; ++col) {
      model = glm::mat4(1.0f);
      model = glm::translate(
          model, glm::vec3((float)(col - (nrColumns / 2)) * spacing,
                           (float)(row - (nrRows / 2)) * spacing, 0.0f));
      pipeline.SetMat4("model", model);
      pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
      sphere_.RenderSphere();
    }
  }

  for (unsigned int i = 0;
       i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
    glm::vec3 newPos =
        lightPositions[i] + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
    newPos = lightPositions[i];
    pipeline.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
    pipeline.SetVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

    model = glm::mat4(1.0f);
    model = glm::translate(model, newPos);
    model = glm::scale(model, glm::vec3(0.5f));
    pipeline.SetMat4("model", model);
    pipeline.SetMat3("normalMatrix",
                     glm::transpose(glm::inverse(glm::mat3(model))));
    sphere_.RenderSphere();
  }

  // render skybox (render as last to prevent overdraw)
  backgroundShader.SetMat4("view", view);
  cubemapsHDR_.DrawHDR();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloHDRCubemap scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
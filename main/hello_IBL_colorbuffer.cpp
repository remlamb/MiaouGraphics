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
#include "PrimitiveObjects.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloIBLColorBuffer final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view textVertexShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr.vert";
  const std::string_view textFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr.frag";

  Pipeline pbr_pipeline;

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

  // glm::vec3 lightPositions[4] = {
  //     glm::vec3(-10.0f, 10.0f, 10.0f),
  //     glm::vec3(10.0f, 10.0f, 10.0f),
  //     glm::vec3(-10.0f, -10.0f, 10.0f),
  //     glm::vec3(10.0f, -10.0f, 10.0f),
  // };
  glm::vec3 lightPositions[1] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
  };
  glm::vec3 lightColors[4] = {
      glm::vec3(80.0f, 200.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

  int nrRows = 7;
  int nrColumns = 7;
  float spacing = 2.5;

  const std::string_view equirectangleVertexShaderFilePath_ =
      "data/shaders/hello_IBL/cubemap_hdr.vert";
  const std::string_view equirectangleFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/cubemap_hdr.frag";

  Pipeline equirectangular_to_cubemap_pipeline;

  const std::string_view backgroundVertexShaderFilePath_ =
      "data/shaders/hello_IBL/background.vert";
  const std::string_view backgroundFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/background.frag";

  Pipeline backgroundShader;

  Texture hdr_cubemap;
  const std::string_view hdr_cubemapFilePath_ =
      "data/textures/xiequ_yuan_hdr/xiequ_yuan.hdr";

  const std::string_view irradianceVertexShaderFilePath_ =
      "data/shaders/hello_IBL/irradiance_convolution.vert";
  const std::string_view irradianceFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/irradiance_convolution.frag";

  Pipeline irradianceShader;

  const std::string_view prefilterVertexShaderFilePath_ =
      "data/shaders/hello_IBL/prefilter.vert";
  const std::string_view prefilterFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/prefilter.frag";

  Pipeline prefilterShader;

  const std::string_view brdfVertexShaderFilePath_ =
      "data/shaders/hello_IBL/brdf.vert";
  const std::string_view brdfFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/brdf.frag";

  Pipeline brdfShader;

  const std::string_view cat_model_path = "data/model/cat/newcat.obj";
  Model cat_;

  Texture catBaseColor;
  const std::string_view catBaseColorFilePath_ =
      "data/model/cat/png_texture/cat_diff.png";

  Texture catNormal;
  const std::string_view catNormalFilePath_ =
      "data/model/cat/png_texture/cat_nor_gl.png";

  Texture catMetallic;
  const std::string_view catMetallicFilePath_ =
      "data/model/cat/png_texture/cat_metal.png";

  Texture catRoughness;
  const std::string_view catRoughnessFilePath_ =
      "data/model/cat/png_texture/cat_rough.png";

  Texture catAo;
  const std::string_view catAoFilePath_ =
      "data/model/cat/png_texture/cat_ao.png";

  const std::string_view michelle_model_path =
      "data/model/Michelle/Michelle.obj";
  Model michelle_;

  Texture mBaseColor;
  const std::string_view mBCFilePath_ =
      "data/model/Michelle/texture/M_Diffuse.png";

  Texture mNormal;
  const std::string_view mNFilePath_ =
      "data/model/Michelle/texture/M_Normal.png";

  Texture mGlossiness;
  const std::string_view mGFilePath_ =
      "data/model/Michelle/texture/M_Glossiness.png";

  Texture AluAlbedo;
  const std::string_view aluAFilePath_ =
      //"data/textures/Aluminum/aluminum_albedo.png";
      "data/textures/Gold/gold_basecolor.png";

  Texture AluNormal;
  const std::string_view aluNFilePath_ =
      //"data/textures/Aluminum/aluminum_normal.png";
      "data/textures/Gold/gold_normal.png";

  Texture AluMetallic;
  const std::string_view aluMFilePath_ =
      //"data/textures/Aluminum/aluminum_metallic.png";
      "data/textures/Gold/gold_metallic.png";

  Texture AluRoughness;
  const std::string_view aluRFilePath_ =
      // "data/textures/Aluminum/aluminum_roughness.png";
      "data/textures/Gold/gold_roughness.png";

  Texture AluAO;
  const std::string_view aluAOFilePath_ =
      "data/textures/Aluminum/aluminum_ao.png";

  // PrimitiveObjects quad_;
  PrimitiveObjects cube_;
  PrimitiveObjects sphere_;

  Cubemaps cubemapsHDR_;

  FrameBuffer colorBuffer;

  const std::string_view screenVertexShaderFilePath_ =
      "data/shaders/hello_framebuffer/framebuffer.vert";
  const std::string_view screenFragmentShaderFilePath_ =
      "data/shaders/hello_framebuffer/framebuffer.frag";

  Pipeline screen_pipeline;

  const std::string_view grass2dVertexShaderFilePath_ =
      "data/shaders/hello_instantiate/moving_grass2d.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_instantiate/moving_grass2d.frag";
  Pipeline grass2d_pipeline;
  Texture grass_texture_;
  const std::string_view grass_texture_FilePath_ =
      "data/textures/stylized_grass.png";
  InstantiateGrass grass_;

  PrimitiveObjects plane_;
  Texture meaAlbedo;
  const std::string_view meaAFilePath_ =
      "data/textures/Meadow/meadow_albedo.png";

  Texture meaNormal;
  const std::string_view meaNFilePath_ =
      "data/textures/Meadow/meadow_normal.png";

  Texture meaMetallic;
  const std::string_view meaMFilePath_ =
      "data/textures/Meadow/meadow_metallic.png";

  Texture meaRoughness;
  const std::string_view meaRFilePath_ =
      "data/textures/Meadow/meadow_roughness.png";

  Texture meaAO;
  const std::string_view meaAOFilePath_ = "data/textures/Meadow/meadow_ao.png";
};

void HelloIBLColorBuffer::Begin() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  // quad_.SetUpQuad();
  cube_.SetUpCube();
  sphere_.SetUpSphere();
  plane_.SetUpPlane();

  grass2d_pipeline.CreateProgram(grass2dVertexShaderFilePath_,
                                 grass2dFragmentShaderFilePath_);
  grass_texture_.is_uv_inverted = false;
  grass_texture_.TextureFromFile(grass_texture_FilePath_);
  grass_.SetUp();

  hdr_cubemap.HDRTextureFromFile(hdr_cubemapFilePath_.data());

  pbr_pipeline.CreateProgram(textVertexShaderFilePath_,
                             textFragmentShaderFilePath_);

  equirectangular_to_cubemap_pipeline.CreateProgram(
      equirectangleVertexShaderFilePath_, equirectangleFragmentShaderFilePath_);

  backgroundShader.CreateProgram(backgroundVertexShaderFilePath_,
                                 backgroundFragmentShaderFilePath_);

  brdfShader.CreateProgram(brdfVertexShaderFilePath_,
                           brdfFragmentShaderFilePath_);

  prefilterShader.CreateProgram(prefilterVertexShaderFilePath_,
                                prefilterFragmentShaderFilePath_);

  irradianceShader.CreateProgram(irradianceVertexShaderFilePath_,
                                 irradianceFragmentShaderFilePath_);

  cat_.loadModel(cat_model_path.data());

  michelle_.loadModel(michelle_model_path.data());

  albedo.TextureFromFile(albedoFilePath_.data());
  normal.TextureFromFile(normalFilePath_.data());
  metallic.TextureFromFile(metallicFilePath_.data());
  roughness.TextureFromFile(roughnessFilePath_.data());
  ao.TextureFromFile(aoFilePath_.data());

  catBaseColor.is_uv_inverted = false;
  catBaseColor.TextureFromFile(catBaseColorFilePath_.data());

  catNormal.is_uv_inverted = false;
  catNormal.TextureFromFile(catNormalFilePath_.data());

  catMetallic.is_uv_inverted = false;
  catMetallic.TextureFromFile(catMetallicFilePath_.data());

  catRoughness.is_uv_inverted = false;
  catRoughness.TextureFromFile(catRoughnessFilePath_.data());

  catAo.is_uv_inverted = false;
  catAo.TextureFromFile(catAoFilePath_.data());

  mBaseColor.is_uv_inverted = false;
  mBaseColor.TextureFromFile(mBCFilePath_.data());
  mGlossiness.is_uv_inverted = false;
  mGlossiness.TextureFromFile(mNFilePath_.data());
  mNormal.is_uv_inverted = false;
  mNormal.TextureFromFile(mNFilePath_.data());

  AluAlbedo.is_uv_inverted = false;
  AluAlbedo.TextureFromFile(aluAFilePath_.data());
  AluMetallic.is_uv_inverted = false;
  AluMetallic.TextureFromFile(aluMFilePath_.data());
  AluNormal.is_uv_inverted = false;
  AluNormal.TextureFromFile(aluNFilePath_.data());
  AluRoughness.is_uv_inverted = false;
  AluRoughness.TextureFromFile(aluRFilePath_.data());
  AluAO.is_uv_inverted = false;
  AluAO.TextureFromFile(aluAOFilePath_.data());

  meaAlbedo.is_uv_inverted = false;
  meaAlbedo.TextureFromFileRepeat(meaAFilePath_.data());
  meaMetallic.is_uv_inverted = false;
  meaMetallic.TextureFromFileRepeat(meaMFilePath_.data());
  meaNormal.is_uv_inverted = false;
  meaNormal.TextureFromFileRepeat(meaNFilePath_.data());
  meaRoughness.is_uv_inverted = false;
  meaRoughness.TextureFromFileRepeat(meaRFilePath_.data());
  meaAO.is_uv_inverted = false;
  meaAO.TextureFromFileRepeat(meaAOFilePath_.data());

  // albedo.BindTexture(GL_TEXTURE3);
  // normal.BindTexture(GL_TEXTURE4);
  // metallic.BindTexture(GL_TEXTURE5);
  // roughness.BindTexture(GL_TEXTURE6);
  // ao.BindTexture(GL_TEXTURE7);

  AluAlbedo.BindTexture(GL_TEXTURE3);
  AluNormal.BindTexture(GL_TEXTURE4);
  AluMetallic.BindTexture(GL_TEXTURE5);
  AluRoughness.BindTexture(GL_TEXTURE6);
  AluAO.BindTexture(GL_TEXTURE7);

  pbr_pipeline.SetInt("irradianceMap", 0);
  pbr_pipeline.SetInt("prefilterMap", 1);
  pbr_pipeline.SetInt("brdfLUT", 2);
  pbr_pipeline.SetInt("albedoMap", 3);
  pbr_pipeline.SetInt("normalMap", 4);
  pbr_pipeline.SetInt("metallicMap", 5);
  pbr_pipeline.SetInt("roughnessMap", 6);
  pbr_pipeline.SetInt("aoMap", 7);

  backgroundShader.SetInt("environmentMap", 0);

  cubemapsHDR_.SetUpHDR(equirectangular_to_cubemap_pipeline, hdr_cubemap);

  cubemapsHDR_.GenerateMipMap();
  cubemapsHDR_.GenerateIrradianceMap(irradianceShader);
  cubemapsHDR_.GeneratePrefilterMap(prefilterShader);
  cubemapsHDR_.GeneratebrdfLUTText(brdfShader);

  projection = glm::perspective(glm::radians(50.0f),
                                static_cast<float>(Engine::screen_width_) /
                                    static_cast<float>(Engine::screen_height_),
                                0.1f, 100.0f);

  pbr_pipeline.SetMat4("projection", projection);
  backgroundShader.SetMat4("projection", projection);

  glViewport(0, 0, Engine::screen_width_, Engine::screen_height_);

  colorBuffer.SetUpColorBuffer();
  screen_pipeline.CreateProgram(screenVertexShaderFilePath_,
                                screenFragmentShaderFilePath_);
}

void HelloIBLColorBuffer::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

void HelloIBLColorBuffer::Update(float dt) {
  colorBuffer.Reset();
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  // render
  // ------
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Enable Face culling only for 3d model
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // render scene, supplying the convoluted irradiance map to the final shader.
  // ------------------------------------------------------------------------------------------
  pbr_pipeline.SetMat4("view", view);
  pbr_pipeline.SetVec3("camPos", camera_.position_);

  // bind pre-computed IBL data
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapsHDR_.irradianceMap);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapsHDR_.prefilterMap);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, cubemapsHDR_.brdfLUTTexture);

  pbr_pipeline.SetInt("irradianceMap", 0);
  pbr_pipeline.SetInt("prefilterMap", 1);
  pbr_pipeline.SetInt("brdfLUT", 2);

  // render rows*column number of spheres with varying metallic/roughness values
  // scaled by rows and columns respectively
  glm::mat4 model = glm::mat4(1.0f);
  for (int row = 0; row < nrRows; ++row) {
    pbr_pipeline.SetFloat("metallic", (float)row / (float)nrRows);
    for (int col = 0; col < nrColumns; ++col) {
      // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces
      // (roughness of 0.0) tend to look a bit off on direct lighting.
      pbr_pipeline.SetFloat(
          "roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

      model = glm::mat4(1.0f);
      model = glm::translate(
          model, glm::vec3((float)(col - (nrColumns / 2)) * spacing,
                           (float)(row - (nrRows / 2)) * spacing, -2.0f));
      pbr_pipeline.SetMat4("model", model);
      pbr_pipeline.SetMat3("normalMatrix",
                           glm::transpose(glm::inverse(glm::mat3(model))));
      // renderSphere();
      // mBaseColor.BindTexture(GL_TEXTURE3);
      // pbr_pipeline.SetInt("albedoMap", 3);
      // mNormal.BindTexture(GL_TEXTURE4);
      // pbr_pipeline.SetInt("normalMap", 4);
      // mGlossiness.BindTexture(GL_TEXTURE6);
      // pbr_pipeline.SetInt("roughnessMap", 6);

      AluAlbedo.BindTexture(GL_TEXTURE3);
      AluNormal.BindTexture(GL_TEXTURE4);
      AluMetallic.BindTexture(GL_TEXTURE5);
      AluRoughness.BindTexture(GL_TEXTURE6);
      AluAO.BindTexture(GL_TEXTURE7);

      pbr_pipeline.SetInt("albedoMap", 3);
      pbr_pipeline.SetInt("normalMap", 4);
      pbr_pipeline.SetInt("metallicMap", 5);
      pbr_pipeline.SetInt("roughnessMap", 6);
      pbr_pipeline.SetInt("aoMap", 7);

      michelle_.Draw(pbr_pipeline.program_);
    }
  }
  model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(8.2f, 8.2f, 8.2f));
  pbr_pipeline.SetMat4("model", model);

  catBaseColor.BindTexture(GL_TEXTURE3);
  pbr_pipeline.SetInt("albedoMap", 3);
  catNormal.BindTexture(GL_TEXTURE4);
  pbr_pipeline.SetInt("normalMap", 4);
  catMetallic.BindTexture(GL_TEXTURE5);
  pbr_pipeline.SetInt("metallicMap", 5);
  catRoughness.BindTexture(GL_TEXTURE6);
  pbr_pipeline.SetInt("roughnessMap", 6);
  catAo.BindTexture(GL_TEXTURE7);
  pbr_pipeline.SetInt("aoMap", 7);

  /* AluAlbedo.BindTexture(GL_TEXTURE3);
   AluNormal.BindTexture(GL_TEXTURE4);
   AluMetallic.BindTexture(GL_TEXTURE5);
   AluRoughness.BindTexture(GL_TEXTURE6);
   AluAO.BindTexture(GL_TEXTURE7);*/

  pbr_pipeline.SetInt("albedoMap", 3);
  pbr_pipeline.SetInt("normalMap", 4);
  pbr_pipeline.SetInt("metallicMap", 5);
  pbr_pipeline.SetInt("roughnessMap", 6);
  pbr_pipeline.SetInt("aoMap", 7);

  cat_.Draw(pbr_pipeline.program_);

  // render light source (simply re-render sphere at light positions)
  // this looks a bit off as we use the same shader, but it'll make their
  // positions obvious and keeps the codeprint small.
  for (unsigned int i = 0;
       i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
    glm::vec3 newPos =
        lightPositions[i] + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
    newPos = lightPositions[i];
    pbr_pipeline.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
    pbr_pipeline.SetVec3("lightColors[" + std::to_string(i) + "]",
                         lightColors[i]);

    model = glm::mat4(1.0f);
    model = glm::translate(model, newPos);
    model = glm::scale(model, glm::vec3(0.5f));
    pbr_pipeline.SetMat4("model", model);
    pbr_pipeline.SetMat3("normalMatrix",
                         glm::transpose(glm::inverse(glm::mat3(model))));
    sphere_.RenderSphere();
  }

  // Disable face culling for 2d and skybox
  glDisable(GL_CULL_FACE);

  model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
  model = glm::translate(model, glm::vec3(1.0f, -1.2f, 1.0f));
  pbr_pipeline.SetMat4("model", model);

  meaAlbedo.BindTexture(GL_TEXTURE3);
  meaNormal.BindTexture(GL_TEXTURE4);
  meaMetallic.BindTexture(GL_TEXTURE5);
  meaRoughness.BindTexture(GL_TEXTURE6);
  meaAO.BindTexture(GL_TEXTURE7);

  pbr_pipeline.SetInt("albedoMap", 3);
  pbr_pipeline.SetInt("normalMap", 4);
  pbr_pipeline.SetInt("metallicMap", 5);
  pbr_pipeline.SetInt("roughnessMap", 6);
  pbr_pipeline.SetInt("aoMap", 7);

  plane_.RenderPlane();

  float dimtime = 2 * timer_;
  float dimwindStrength = 0.18f;  // Adjust as needed
  model = glm::mat4(1.0f);
  grass2d_pipeline.SetFloat("time", dimtime);
  grass2d_pipeline.SetFloat("windStrength", dimwindStrength);
  grass_texture_.BindTexture(GL_TEXTURE10);
  grass2d_pipeline.SetInt("texture1", 10);
  model = glm::scale(model, glm::vec3(8, 8, 8));
  grass2d_pipeline.SetMat4("model", model);
  grass2d_pipeline.SetMat4("view", view);
  grass2d_pipeline.SetMat4("projection", projection);
  grass_.Render();

  // render skybox (render as last to prevent overdraw)
  backgroundShader.SetMat4("view", view);
  cubemapsHDR_.DrawHDR();

  colorBuffer.Clear();
  screen_pipeline.SetInt("PostProcessingType", colorBuffer.post_process_type_);
  screen_pipeline.SetInt("screenTexture", 0);
  colorBuffer.Draw();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloIBLColorBuffer scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
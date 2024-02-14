#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "Bloom.h"
#include "Camera.h"
#include "Cubemaps.h"
#include "FrameBuffer.h"
#include "InstantiateGrass.h"
#include "Model.h"
#include "PrimitiveObjects.h"
#include "Shadowmap.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloFinalSceneSSAO final : public Scene {
 public:
  void CreatePipelines();
  void LoadModels();
  void LoadTextures();
  void Begin() override;
  void DrawImGui() override;
  void End() override;
  void Update(float dt) override;
  void OnEvent(const SDL_Event& event) override;
  void renderScene(Pipeline& shader);

 private:
  const std::string_view textVertexShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr_with_shadow_multiplelight.vert";
  const std::string_view textFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr_with_shadow_SSAO.frag";

  Pipeline pbr_pipeline;
  Camera camera_;

  const std::string_view pbrcustomFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr_customValue.frag";
  Pipeline pbr_custom;

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

  glm::vec3 lightPosition = glm::vec3(-1.6f, -0.6f, -2.6f);
  float lightRed = 0.01f;
  float lightGreen = 2.8f;
  float lightBlue = 10.0f;
  glm::vec3 lightColor = glm::vec3(lightRed, lightGreen, lightBlue);

  glm::vec3 directionalLightPosition = glm::vec3(-1.0f, 1.0f, 0.0f);
  glm::vec3 directionalLightDirection =
      glm::normalize(glm::vec3(-4.0f) - directionalLightPosition);

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
      "data/model/cat/png_texture/cat_diff_2k.png";

  Texture catNormal;
  const std::string_view catNormalFilePath_ =
      "data/model/cat/png_texture/cat_nor_2k.png";

  Texture catMetallic;
  const std::string_view catMetallicFilePath_ =
      "data/model/cat/png_texture/cat_metal_2k.png";

  Texture catRoughness;
  const std::string_view catRoughnessFilePath_ =
      "data/model/cat/png_texture/cat_rough_2k.png";

  Texture catAo;
  const std::string_view catAoFilePath_ =
      "data/model/cat/png_texture/cat_ao_2k.png";

  const std::string_view michelle_model_path = "data/model/Flower/Flower.obj";
  Model flower_;

  const std::string_view tea_model_path = "data/model/Tea/tea.obj";
  Model tea_;

  const std::string_view table_model_path = "data/model/Table/table.obj";
  Model table_;

  Texture TableAlbedo;
  const std::string_view tableAFilePath_ =
      "data/model/Table/cherrywood-albedo.png";

  Texture TableNormal;
  const std::string_view tableNFilePath_ =
      "data/model/Table/cherrywood-Normal.png";

  Texture TableRoughness;
  const std::string_view tableRFilePath_ =
      "data/model/Table/cherrywood-Roughness.png";

  Texture TableMetallic;
  const std::string_view tableMFilePath_ =
      "data/model/Table/cherrywood-Metallic.png";

  Texture TableAO;
  const std::string_view tableAOFilePath_ =
      "data/model/Table/cherrywood-ao.png";

  const std::string_view lantern_model_path =
      "data/model/lantern/chinese_lantern_part1.obj";
  Model lantern_;
  const std::string_view lantern_part2_model_path =
      "data/model/lantern/chinese_lantern_part2.obj";
  Model lantern_part2_;

  Pipeline blending;
  const std::string_view blendingVertexShaderFilePath_ =
      "data/shaders/hello_model/model.vert";
  const std::string_view blendingFragmentShaderFilePath_ =
      "data/shaders/hello_model/model.frag";

  Texture lanternAlbedo;
  const std::string_view lanternAFilePath_ =
      "data/model/lantern/lantern_BaseColor2k.png";

  Texture lanternNormal;
  const std::string_view lanternNFilePath_ =
      "data/model/lantern/lantern_Normal2k.png";

  Texture lanternRoughness;
  const std::string_view lanternRFilePath_ =
      "data/model/lantern/lantern_Roughness2k.png";

  Texture lanternMetallic;
  const std::string_view lanternMFilePath_ =
      "data/model/lantern/lantern_Metallic2k.png";

  Texture goldAlbedo;
  const std::string_view goldAFilePath =
      "data/textures/Gold/gold_basecolor.png";

  Texture goldNormal;
  const std::string_view goldNFilePath_ = "data/textures/Gold/gold_normal.png";

  Texture goldMetallic;
  const std::string_view goldMFilePath_ =
      "data/textures/Gold/gold_metallic.png";

  Texture goldRoughness;
  const std::string_view goldRFilePath_ =
      "data/textures/Gold/gold_roughness.png";

  Texture goldAO;
  const std::string_view goldAOFilePath_ =
      "data/textures/Aluminum/aluminum_ao.png";

  PrimitiveObjects cube_;
  PrimitiveObjects sphere_;

  Cubemaps cubemapsHDR_;

  Pipeline screen_pipeline;

  const std::string_view grass2dVertexShaderFilePath_ =
      "data/shaders/hello_IBL_model/grass_shadow.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr_grass.frag";
  Pipeline grass2d_pipeline;

  Texture grass_texture_;
  const std::string_view grass_texture_FilePath_ = "data/textures/grass.png";
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

  // DEPTH MAP SHADOW
  Pipeline simpleDepthShader;
  const std::string_view shadow_mapping_depthVertexShaderFilePath_ =
      "data/shaders/hello_shadowmapping/shadow_mapping_depth.vert";
  const std::string_view shadow_mapping_depthFragmentShaderFilePath_ =
      "data/shaders/hello_shadowmapping/shadow_mapping_depth.frag";

  // DEPTH MAP FOR INSTANTATE
  Pipeline simpleDepthInstantiateShader;
  const std::string_view instantiate_shadow_mapping_depthVertexShaderFilePath_ =
      "data/shaders/hello_shadowmapping/instantiate_shadow_mapping.vert";

  Shadowmap shadowmap_;

  const std::string_view light_boxVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/bloom.vert";
  const std::string_view light_boxFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/light_box.frag";
  Pipeline shaderLight;

  PrimitiveObjects lightCube_;

  bool bloom = true;
  float exposure = 1.0f;
  float bloomStrength_ = 0.18f;
  int programChoice = 3;
  float bloomFilterRadius = 0.008f;
  unsigned int hdrFBO;
  unsigned int colorBuffers[2];

  unsigned int rboDepth;
  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

  unsigned int pingpongFBO[2];
  unsigned int pingpongColorbuffers[2];

  BloomRenderer bloomRenderer;
  PrimitiveObjects bloomquad_;

  const std::string_view bloomFinalVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/bloom_final.vert";
  const std::string_view bloomFinalFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/colorshader.frag";
  Pipeline shaderBloomFinal;

  // IMGUI Elements
  bool main_window = true;
  bool show_another_window = true;
  bool instantiate_grass_ = true;
  bool black_white_filter_ = false;

  float flower_roughness = 0.1f;
  float flower_metalic = 0.9f;
  float flower_ao = 1.0f;

  float directional_light_intensity_ = 1.0f;

  float ssaoRadius_ = 0.12f;
  float ssaoBias_ = 0.025f;

  // SSAO
  const std::string_view GeometryPassVertexShaderFilePath_ =
      "data/shaders/hello_SSAO/ssao_geometry.vert";
  const std::string_view GeometryPassFragmentShaderFilePath_ =
      "data/shaders/hello_SSAO/ssao_geometry.frag";
  Pipeline shaderGeometryPass;

  const std::string_view LightingPassFragmentShaderFilePath_ =
      "data/shaders/hello_SSAO/ssao_lighting.frag";
  Pipeline shaderLightingPass;

  const std::string_view SSAOVertexShaderFilePath_ =
      "data/shaders/hello_SSAO/ssao.vert";
  const std::string_view SSAOFragmentShaderFilePath_ =
      "data/shaders/hello_SSAO/ssao.frag";
  Pipeline shaderSSAO;

  const std::string_view SSAOBlurFragmentShaderFilePath_ =
      "data/shaders/hello_SSAO/ssao_blur.frag";
  Pipeline shaderSSAOBlur;

  unsigned int gBuffer;
  unsigned int gPosition, gNormal, gAlbedo;
  unsigned int SSAOattachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                     GL_COLOR_ATTACHMENT2};
  unsigned int SSAOrboDepth;
  unsigned int ssaoFBO, ssaoBlurFBO;
  unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

  std::vector<glm::vec3> ssaoNoise;
  unsigned int noiseTexture;

  std::vector<glm::vec3> ssaoKernel;

  PrimitiveObjects SSAOcube_;
  PrimitiveObjects SSAOquad_;
};

void HelloFinalSceneSSAO::CreatePipelines() {
  grass2d_pipeline.CreateProgram(grass2dVertexShaderFilePath_,
                                 grass2dFragmentShaderFilePath_);
  pbr_pipeline.CreateProgram(SSAOVertexShaderFilePath_,
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

  simpleDepthShader.CreateProgram(shadow_mapping_depthVertexShaderFilePath_,
                                  shadow_mapping_depthFragmentShaderFilePath_);

  shaderLight.CreateProgram(light_boxVertexShaderFilePath_,
                            light_boxFragmentShaderFilePath_);

  blending.CreateProgram(blendingVertexShaderFilePath_,
                         blendingFragmentShaderFilePath_);

  simpleDepthInstantiateShader.CreateProgram(
      instantiate_shadow_mapping_depthVertexShaderFilePath_,
      shadow_mapping_depthFragmentShaderFilePath_);

  pbr_custom.CreateProgram(textVertexShaderFilePath_,
                           pbrcustomFragmentShaderFilePath_);

  shaderGeometryPass.CreateProgram(GeometryPassVertexShaderFilePath_,
                                   GeometryPassFragmentShaderFilePath_);

  shaderLightingPass.CreateProgram(SSAOVertexShaderFilePath_,
                                   LightingPassFragmentShaderFilePath_);

  shaderSSAO.CreateProgram(SSAOVertexShaderFilePath_,
                           SSAOFragmentShaderFilePath_);

  shaderSSAOBlur.CreateProgram(SSAOVertexShaderFilePath_,
                               SSAOBlurFragmentShaderFilePath_);
}

void HelloFinalSceneSSAO::LoadModels() {
  cat_.loadModel(cat_model_path.data());
  flower_.loadModel(michelle_model_path.data());
  table_.loadModel(table_model_path.data());
  lantern_.loadModel(lantern_model_path.data());
  lantern_part2_.loadModel(lantern_part2_model_path.data());
  tea_.loadModel(tea_model_path.data());
}

void HelloFinalSceneSSAO::LoadTextures() {
  catBaseColor.HDRTextureFromFile(catBaseColorFilePath_.data(), false);
  catNormal.TextureFromFile(catNormalFilePath_.data(), false);
  catMetallic.TextureFromFile(catMetallicFilePath_.data(), false);
  catRoughness.TextureFromFile(catRoughnessFilePath_.data(), false);
  catAo.TextureFromFile(catAoFilePath_.data(), false);

  goldAlbedo.HDRTextureFromFile(goldAFilePath.data(), false);
  goldMetallic.TextureFromFile(goldMFilePath_.data(), false);
  goldNormal.TextureFromFile(goldNFilePath_.data(), false);
  goldRoughness.TextureFromFile(goldRFilePath_.data(), false);
  goldAO.TextureFromFile(goldAOFilePath_.data(), false);

  meaAlbedo.HDRTextureFromFileRepeat(meaAFilePath_.data());
  meaMetallic.TextureFromFileRepeat(meaMFilePath_.data());
  meaNormal.TextureFromFileRepeat(meaNFilePath_.data());
  meaRoughness.TextureFromFileRepeat(meaRFilePath_.data());
  meaAO.TextureFromFileRepeat(meaAOFilePath_.data());

  TableAlbedo.HDRTextureFromFile(tableAFilePath_.data(), false);
  TableNormal.TextureFromFile(tableNFilePath_.data(), false);
  TableMetallic.TextureFromFile(tableMFilePath_.data(), false);
  TableRoughness.TextureFromFile(tableRFilePath_.data(), false);
  TableAO.TextureFromFile(tableAOFilePath_.data(), false);

  lanternAlbedo.HDRTextureFromFile(lanternAFilePath_.data(), false);
  lanternNormal.TextureFromFile(lanternNFilePath_.data(), false);
  lanternMetallic.TextureFromFile(lanternMFilePath_.data(), false);
  lanternRoughness.TextureFromFile(lanternRFilePath_.data(), false);

  grass_texture_.TextureFromFile(grass_texture_FilePath_, false);
}

float ourLerp(float a, float b, float f) { return a + f * (b - a); }

void HelloFinalSceneSSAO::Begin() {
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glEnable(GL_DEPTH_TEST);

  // quad_.SetUpQuad();
  cube_.SetUpCube();
  sphere_.SetUpSphere();
  lightCube_.SetUpCubeOpenGL();
  plane_.SetUpPlane();
  bloomquad_.SetUpQuadbrdf();
  SSAOquad_.SetUpQuadbrdf();

  CreatePipelines();
  LoadModels();
  LoadTextures();

  grass_.SetUp();
  hdr_cubemap.HDRTextureFromFile(hdr_cubemapFilePath_.data());
  shadowmap_.SetUp();

  pbr_pipeline.SetInt("irradianceMap", 0);
  pbr_pipeline.SetInt("prefilterMap", 1);
  pbr_pipeline.SetInt("brdfLUT", 2);
  pbr_pipeline.SetInt("gPosition", 3);
  pbr_pipeline.SetInt("gNormal", 4);
  pbr_pipeline.SetInt("gAlbedo", 5);
  pbr_pipeline.SetInt("SSAOMap", 6);
  pbr_pipeline.SetInt("shadowMap", 7);

  shaderGeometryPass.SetInt("albedoMap", 3);
  shaderGeometryPass.SetInt("normalMap", 4);
  shaderGeometryPass.SetInt("metallicMap", 5);
  shaderGeometryPass.SetInt("roughnessMap", 6);
  shaderGeometryPass.SetInt("aoMap", 7);

  pbr_custom.SetInt("irradianceMap", 0);
  pbr_custom.SetInt("prefilterMap", 1);
  pbr_custom.SetInt("brdfLUT", 2);
  pbr_custom.SetInt("albedoMap", 3);
  pbr_custom.SetInt("normalMap", 4);
  pbr_custom.SetInt("shadowMap", 10);
  pbr_custom.SetVec3("lightPositions[0]", lightPosition);
  pbr_custom.SetVec3("directionalLightDirection", directionalLightDirection);
  pbr_custom.SetVec3("directionalLightColor",
                     glm::vec3(directional_light_intensity_));

  grass2d_pipeline.SetInt("irradianceMap", 0);
  grass2d_pipeline.SetInt("prefilterMap", 1);
  grass2d_pipeline.SetInt("brdfLUT", 2);
  grass2d_pipeline.SetInt("albedoMap", 3);
  grass2d_pipeline.SetInt("normalMap", 4);
  grass2d_pipeline.SetInt("shadowMap", 10);
  grass2d_pipeline.SetVec3("lightPositions[0]", lightPosition);
  grass2d_pipeline.SetVec3("directionalLightDirection",
                           directionalLightDirection);
  grass2d_pipeline.SetVec3("directionalLightColor",
                           glm::vec3(directional_light_intensity_));

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
  pbr_custom.SetMat4("projection", projection);
  backgroundShader.SetMat4("projection", projection);
  shaderLight.SetMat4("projection", projection);

  glViewport(0, 0, Engine::screen_width_, Engine::screen_height_);

  pbr_pipeline.SetVec3("lightPositions[0]", lightPosition);
  pbr_pipeline.SetVec3("lightColors[0]", lightColor);

  pbr_pipeline.SetVec3("directionalLightDirection", directionalLightDirection);
  pbr_pipeline.SetVec3("directionalLightColor",
                       glm::vec3(directional_light_intensity_));

  //___NEW___
  shaderBloomFinal.CreateProgram(bloomFinalVertexShaderFilePath_,
                                 bloomFinalFragmentShaderFilePath_);
  shaderBloomFinal.SetInt("scene", 0);
  shaderBloomFinal.SetInt("bloomBlur", 1);
  // configure (floating point) framebuffers
  // ---------------------------------------
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  // create 2 floating point color buffers (1 for normal rendering, other for
  // brightness threshold values)
  glGenTextures(2, colorBuffers);
  for (unsigned int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Engine::screen_width_,
                 Engine::screen_height_, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would
                            // otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, colorBuffers[i], 0);
  }

  // create and attach depth buffer (renderbuffer)
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        Engine::screen_width_, Engine::screen_height_);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);
  // tell OpenGL which color attachments we'll use (of this framebuffer) for
  // rendering

  glDrawBuffers(2, attachments);
  // finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // ping-pong-framebuffer for blurring

  glGenFramebuffers(2, pingpongFBO);
  glGenTextures(2, pingpongColorbuffers);
  for (unsigned int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Engine::screen_width_,
                 Engine::screen_height_, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would
                            // otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pingpongColorbuffers[i], 0);
    // also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;
  }

  // shader configuration
  // --------------------
  // shader.SetInt("diffuseTexture", 0);
  // shaderBlur.SetInt("image", 0);
  shaderBloomFinal.SetInt("scene", 0);
  shaderBloomFinal.SetInt("bloomBlur", 1);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // bloom renderer
  // --------------
  bloomRenderer.mDownsampleShader.SetInt("srcTexture", 0);
  bloomRenderer.mUpsampleShader.SetInt("srcTexture", 0);
  bloomRenderer.Init(Engine::screen_width_, Engine::screen_height_);

  // configure g-buffer framebuffer SSAO
  // ------------------------------

  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  // position color buffer
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Engine::screen_width_,
               Engine::screen_height_, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);
  // normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Engine::screen_width_,
               Engine::screen_height_, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);
  // color + specular color buffer
  glGenTextures(1, &gAlbedo);
  glBindTexture(GL_TEXTURE_2D, gAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Engine::screen_width_,
               Engine::screen_height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedo, 0);
  // tell OpenGL which color attachments we'll use (of this framebuffer) for
  // rendering

  glDrawBuffers(3, SSAOattachments);
  // create and attach depth buffer (renderbuffer)

  glGenRenderbuffers(1, &SSAOrboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, SSAOrboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        Engine::screen_width_, Engine::screen_height_);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, SSAOrboDepth);
  // finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // also create framebuffer to hold SSAO processing stage
  // -----------------------------------------------------

  glGenFramebuffers(1, &ssaoFBO);
  glGenFramebuffers(1, &ssaoBlurFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

  // SSAO color buffer
  glGenTextures(1, &ssaoColorBuffer);
  glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Engine::screen_width_,
               Engine::screen_height_, 0, GL_RED, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         ssaoColorBuffer, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "SSAO Framebuffer not complete!" << std::endl;
  // and blur stage
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
  glGenTextures(1, &ssaoColorBufferBlur);
  glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Engine::screen_width_,
               Engine::screen_height_, 0, GL_RED, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         ssaoColorBufferBlur, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // generate sample kernel
  // ----------------------
  std::uniform_real_distribution<GLfloat> randomFloats(
      0.0, 1.0);  // generates random floats between 0.0 and 1.0
  std::default_random_engine generator;

  for (unsigned int i = 0; i < 64; ++i) {
    glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0,
                     randomFloats(generator) * 2.0 - 1.0,
                     randomFloats(generator));
    sample = glm::normalize(sample);
    sample *= randomFloats(generator);
    float scale = float(i) / 64.0f;

    // scale samples s.t. they're more aligned to center of kernel
    scale = ourLerp(0.1f, 1.0f, scale * scale);
    sample *= scale;
    ssaoKernel.push_back(sample);
  }

  // generate noise texture
  // ----------------------

  for (unsigned int i = 0; i < 16; i++) {
    glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    0.0f);  // rotate around z-axis (in tangent space)
    ssaoNoise.push_back(noise);
  }

  glGenTextures(1, &noiseTexture);
  glBindTexture(GL_TEXTURE_2D, noiseTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT,
               &ssaoNoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // shader configuration
  // --------------------
  shaderLightingPass.SetInt("gPosition", 0);
  shaderLightingPass.SetInt("gNormal", 1);
  shaderLightingPass.SetInt("gAlbedo", 2);
  shaderLightingPass.SetInt("ssao", 3);
  shaderSSAO.SetInt("gPosition", 0);
  shaderSSAO.SetInt("gNormal", 1);
  shaderSSAO.SetInt("texNoise", 2);
  shaderSSAOBlur.SetInt("ssaoInput", 0);
}

void HelloFinalSceneSSAO::DrawImGui() {
  if (main_window) {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Miaou Settings :");

    ImGui::TextColored(ImVec4(0.5f, 0.9f, 0.65f, 1.0f),
                       "Welcome to Xiequ Yuan !");
    if (ImGui::CollapsingHeader("Camera control")) {
      ImGui::Text("W : move toward, S : move Backward");
      ImGui::Text("A : move left, D : move right");
      ImGui::Text("SPACE : move Up, CTRL : move Down");
      ImGui::Text("Camera rotate with the mouse position");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "R : Freeze Camera Rotation");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "T : reActive Camera Rotation");
    }

    if (ImGui::CollapsingHeader("Materials Settings : ")) {
      ImGui::Text("Change Flower Value : ");
      ImGui::SliderFloat("Roughness", &flower_roughness, 0.05f, 1.0f);
      ImGui::SliderFloat("Metalic", &flower_metalic, 0.05f, 1.0f);
      ImGui::SliderFloat("AO", &flower_ao, 0.05f, 1.0f);
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "R : Freeze Camera Rotation");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "T : reActive Camera Rotation");
    }

    if (ImGui::CollapsingHeader("Instantiate")) {
      ImGui::Checkbox("Show Grass", &instantiate_grass_);
    }

    if (ImGui::CollapsingHeader("Lightning")) {
      ImGui::Text("Change Point Light Color : ");
      ImGui::SliderFloat("Red", &lightRed, 0.01f, 10.0f);
      ImGui::SliderFloat("Green", &lightGreen, 0.01f, 10.0f);
      ImGui::SliderFloat("Blue", &lightBlue, 0.01f, 10.0f);
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "R : Freeze Camera Rotation");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "T : reActive Camera Rotation");

      ImGui::Text(" ");
      ImGui::Text("3 Presets : ");
      if (ImGui::Button("Strong Blue")) {
        lightRed = 0.01f;
        lightGreen = 2.8f;
        lightBlue = 10.0f;
        directional_light_intensity_ = 1.2f;
        ssaoRadius_ = 0.2f;
        bloomStrength_ = 0.12f;
      }
      ImGui::SameLine();
      if (ImGui::Button("Powerfull Pink")) {
        lightRed = 10.0f;
        lightGreen = 2.5f;
        lightBlue = 10.0f;
        directional_light_intensity_ = 6.0f;
        ssaoRadius_ = 0.28f;
        bloomStrength_ = 0.28f;
      }
      ImGui::SameLine();
      if (ImGui::Button("Ambient Purple")) {
        lightRed = 4.3f;
        lightGreen = 1.5f;
        lightBlue = 5.7f;
        directional_light_intensity_ = 1.0f;
        ssaoRadius_ = 0.12f;
        bloomStrength_ = 0.18f;
      }
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "Carefull this change all value !");

      ImGui::Text(" ");
      ImGui::Text("Change Directional light : ");
      ImGui::SliderFloat("Intensity", &directional_light_intensity_, 1.0f,
                         10.0f);
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "R : Freeze Camera Rotation");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "T : reActive Camera Rotation");
    }

    if (ImGui::CollapsingHeader("SSAO")) {
      ImGui::SliderFloat("Radius", &ssaoRadius_, 0.01f, 0.5f);
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "R : Freeze Camera Rotation");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "T : reActive Camera Rotation");
    }

    if (ImGui::CollapsingHeader("Post Processing")) {
      ImGui::Text("Bloom Effect : ");
      ImGui::SliderFloat("Strength", &bloomStrength_, 0.01f, 0.40f);
      ImGui::Checkbox("Black and White Filter", &black_white_filter_);
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "R : Freeze Camera Rotation");
      ImGui::TextColored(ImVec4(1.5f, 0.8f, 2.5f, 1.0f),
                         "T : reActive Camera Rotation");
    }
  }
}

void HelloFinalSceneSSAO::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

void HelloFinalSceneSSAO::renderScene(Pipeline& shader) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.6f, -0.8f, -1.8f));
  model = glm::rotate(model, 3.14f, glm::vec3(0.0f, -1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(3.4f));
  shader.SetMat4("model", model);
  shader.SetBool("isCustomMaterial", true);
  shader.SetFloat("metallic", flower_metalic);
  shader.SetFloat("roughness", flower_roughness);
  shader.SetFloat("ao", flower_ao);
  goldAlbedo.BindTextureHDR(GL_TEXTURE3);
  goldNormal.BindTexture(GL_TEXTURE4);
  flower_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(1.4f, -0.92f, -2.4f));
  model = glm::rotate(model, 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.2f));
  shader.SetMat4("model", model);
  shader.SetBool("isCustomMaterial", false);
  catBaseColor.BindTextureHDR(GL_TEXTURE3);
  catNormal.BindTexture(GL_TEXTURE4);
  catMetallic.BindTexture(GL_TEXTURE5);
  catRoughness.BindTexture(GL_TEXTURE6);
  catAo.BindTexture(GL_TEXTURE7);
  cat_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -2.2f, -2.2f));
  model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.6f));
  model = glm::rotate(model, 1.58f, glm::vec3(0.0f, 1.0f, 0.0f));
  shader.SetMat4("model", model);
  TableAlbedo.BindTextureHDR(GL_TEXTURE3);
  TableNormal.BindTexture(GL_TEXTURE4);
  TableMetallic.BindTexture(GL_TEXTURE5);
  TableRoughness.BindTexture(GL_TEXTURE6);
  TableAO.BindTexture(GL_TEXTURE7);
  table_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.6f, -0.52f, -2.6f));
  model = glm::scale(model, glm::vec3(2.0f));
  shader.SetMat4("model", model);
  lanternAlbedo.BindTextureHDR(GL_TEXTURE3);
  lanternNormal.BindTexture(GL_TEXTURE4);
  lanternMetallic.BindTexture(GL_TEXTURE5);
  lanternRoughness.BindTexture(GL_TEXTURE6);
  goldMetallic.BindTexture(GL_TEXTURE7);
  lantern_.Draw(shader.program_);
  goldAlbedo.BindTextureHDR(GL_TEXTURE3);
  goldNormal.BindTexture(GL_TEXTURE4);
  goldMetallic.BindTexture(GL_TEXTURE5);
  goldRoughness.BindTexture(GL_TEXTURE6);
  goldMetallic.BindTexture(GL_TEXTURE7);
  lantern_part2_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-0.6f, -0.9f, -1.8f));
  model = glm::scale(model, glm::vec3(2.8f));
  shader.SetMat4("model", model);
  goldAlbedo.BindTextureHDR(GL_TEXTURE3);
  goldNormal.BindTexture(GL_TEXTURE4);
  goldMetallic.BindTexture(GL_TEXTURE5);
  goldRoughness.BindTexture(GL_TEXTURE6);
  goldMetallic.BindTexture(GL_TEXTURE7);
  tea_.Draw(shader.program_);

  // No Point Light on the plane, only for this scene
  glDisable(GL_CULL_FACE);
  model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(0.4f));
  model = glm::translate(model, glm::vec3(1.0f, -4.0f, 0.0f));
  shader.SetMat4("model", model);
  shader.SetVec3("lightColors[0]", glm::vec3(0.0f));
  meaAlbedo.BindTextureHDR(GL_TEXTURE3);
  meaNormal.BindTexture(GL_TEXTURE4);
  meaMetallic.BindTexture(GL_TEXTURE5);
  meaRoughness.BindTexture(GL_TEXTURE6);
  meaAO.BindTexture(GL_TEXTURE7);
  plane_.RenderPlane();
}

void HelloFinalSceneSSAO::Update(float dt) {
  // colorBuffer.Reset();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  lightColor = glm::vec3(lightRed, lightGreen, lightBlue);
  pbr_pipeline.SetVec3("lightColors[0]", lightColor);
  pbr_custom.SetVec3("lightColors[0]", lightColor);
  grass2d_pipeline.SetVec3("lightColors[0]", lightColor);

  pbr_pipeline.SetVec3("directionalLightColor",
                       glm::vec3(directional_light_intensity_));
  pbr_custom.SetVec3("directionalLightColor",
                     glm::vec3(directional_light_intensity_));
  grass2d_pipeline.SetVec3("directionalLightColor",
                           glm::vec3(directional_light_intensity_));

  // render for shadow map
  // ------
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shadowmap_.GenerateShadowMap(directionalLightPosition,
                               directionalLightDirection);
  simpleDepthShader.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  renderScene(simpleDepthShader);

  //// INSTANTIATE ELEMENTS ON THE SHADOW MAP
  // simpleDepthInstantiateShader.SetMat4("lightSpaceMatrix",
  //                                      shadowmap_.lightSpaceMatrix);
  // if (instantiate_grass_) {
  //   grass_.Render();
  // }

  shadowmap_.Reset();

  pbr_pipeline.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  pbr_pipeline.SetVec3("viewPos", camera_.position_);

  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, shadowmap_.depthMap);

  // 1. geometry pass: render scene's geometry/color data into gbuffer
  // -----------------------------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClearColor(0.0, 0.0, 0.0,
               1.0);  // keep it black so it doesn't leak into g-buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Enable Face culling only for 3d model
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  shaderGeometryPass.SetMat4("projection", projection);
  shaderGeometryPass.SetMat4("view", view);
  shaderGeometryPass.SetInt("invertedNormals", 0);
  renderScene(shaderGeometryPass);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 2. generate SSAO texture
  // ------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
  glClear(GL_COLOR_BUFFER_BIT);
  // Send kernel + rotation
  for (unsigned int i = 0; i < 64; ++i) {
    shaderSSAO.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
  }
  shaderSSAO.SetFloat("radius", ssaoRadius_);
  shaderSSAO.SetFloat("bias", ssaoBias_);
  shaderSSAO.SetMat4("projection", projection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, noiseTexture);
  SSAOquad_.RenderQuadbrdf();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 3. blur SSAO texture to remove noise
  // ------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
  glClear(GL_COLOR_BUFFER_BIT);
  shaderSSAOBlur.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
  SSAOquad_.RenderQuadbrdf();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //// 4. lighting pass
  ///-----------------------------------------------------------------------------------------------------
  // 1. render scene into floating point framebuffer
  // -----------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render scene, supplying the convoluted irradiance map to the final shader.
  // ------------------------------------------------------------------------------------------
  pbr_pipeline.SetMat4("view", view);
  pbr_pipeline.SetVec3("camPos", camera_.position_);
  // grass2d_pipeline.SetVec3("camPos", camera_.position_);$
  pbr_pipeline.SetMat4("inverseViewMatrix", glm::inverse(view));

  pbr_pipeline.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);

  // bind pre-computed IBL data
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapsHDR_.irradianceMap);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapsHDR_.prefilterMap);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, cubemapsHDR_.brdfLUTTexture);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, gAlbedo);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, shadowmap_.depthMap);

  SSAOquad_.RenderQuadbrdf();

  // Disable face culling for 2d and skybox and lantern because of transparancy
  // glDisable(GL_CULL_FACE);

  // model = glm::mat4(1.0f);
  // model = glm::translate(model, glm::vec3(-1.6f, -0.52f, -2.6f));
  // model = glm::scale(model, glm::vec3(2.0f));
  // pbr_pipeline.SetMat4("model", model);
  // pbr_pipeline.SetMat3("normalMatrix",
  //                      glm::transpose(glm::inverse(glm::mat3(model))));

  // lantern_.Draw(pbr_pipeline.program_);
  // lantern_part2_.Draw(pbr_pipeline.program_);

  // model = glm::mat4(1.0f);
  // model = glm::scale(model, glm::vec3(0.4f));
  // model = glm::translate(model, glm::vec3(1.0f, -4.0f, 0.0f));
  // pbr_pipeline.SetMat4("model", model);
  // pbr_pipeline.SetMat3("normalMatrix",
  //                      glm::transpose(glm::inverse(glm::mat3(model))));

  //// DON'T WANT POINT LIGHT IN PLANE
  // pbr_pipeline.SetVec3("lightColors[0]", glm::vec3(0.0f));
  // plane_.RenderPlane();

  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
  glBlitFramebuffer(0, 0, Engine::screen_width_, Engine::screen_height_, 0, 0,
                    Engine::screen_width_, Engine::screen_height_,
                    GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(lightPosition));
  model = glm::scale(model, glm::vec3(0.2f));
  model = glm::scale(model, glm::vec3(0.4f, 1.0f, 0.4f));
  shaderLight.SetMat4("model", model);
  shaderLight.SetVec3("lightColor", lightColor);
  shaderLight.SetMat4("view", view);
  sphere_.RenderSphere();

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  if (instantiate_grass_) {
    float dimtime = 2 * timer_;
    float dimwindStrength = 0.18f;  // Adjust as needed
    model = glm::mat4(1.0f);
    grass2d_pipeline.SetFloat("time", dimtime);
    grass2d_pipeline.SetFloat("windStrength", dimwindStrength);
    grass_texture_.BindTexture(GL_TEXTURE8);
    grass2d_pipeline.SetInt("albedoMap", 8);
    grass2d_pipeline.SetMat4("model", model);
    grass2d_pipeline.SetMat4("view", view);
    grass2d_pipeline.SetMat4("projection", projection);
    grass2d_pipeline.SetVec3("viewPos", camera_.position_);
    goldNormal.BindTexture(GL_TEXTURE4);
    grass2d_pipeline.SetInt("normalMap", 4);
    grass2d_pipeline.SetFloat("metallic", 0.01f);
    grass2d_pipeline.SetFloat("roughness", 1.0f);
    grass2d_pipeline.SetFloat("ao", 0.4f);
    grass2d_pipeline.SetVec3("aNormal", glm::vec3(0.0f, -1.0f, 0.0f));
    grass2d_pipeline.SetMat3("normalMatrix",
                             glm::transpose(glm::inverse(glm::mat3(model))));
    grass2d_pipeline.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
    grass_.Render();
  }

  // render skybox (render as last to prevent overdraw)

  glDepthFunc(GL_LEQUAL);

  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  backgroundShader.SetMat4("view", view);
  backgroundShader.SetMat4("projection", projection);
  cubemapsHDR_.DrawHDR();

  //  use unthresholded bloom with progressive downsample/upsampling
  // -------------------------------------------------------------------
  bloomRenderer.RenderBloomTexture(colorBuffers[1], bloomFilterRadius);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  shaderBloomFinal.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());

  shaderBloomFinal.SetInt("programChoice", programChoice);
  shaderBloomFinal.SetFloat("exposure", exposure);
  shaderBloomFinal.SetFloat("bloomStrength", bloomStrength_);
  shaderBloomFinal.SetBool("BnWFilter", black_white_filter_);
  bloomquad_.RenderQuadbrdf();
}

void HelloFinalSceneSSAO::OnEvent(const SDL_Event& event) {
  switch (event.type) {
    case SDL_WINDOWEVENT: {
      switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
          const auto new_size =
              glm::uvec2(event.window.data1, event.window.data2);

          glBindRenderbuffer(GL_RENDERBUFFER, SSAOrboDepth);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT,
                                new_size.x, new_size.y);

          glBindTexture(GL_TEXTURE_2D, gPosition);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_size.x, new_size.y, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, NULL);
          glBindTexture(GL_TEXTURE_2D, gNormal);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_size.x, new_size.y, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, NULL);
          glBindTexture(GL_TEXTURE_2D, gAlbedo);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_size.x, new_size.y, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, NULL);

          glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT,
                                new_size.x, new_size.y);

          glBindRenderbuffer(GL_RENDERBUFFER, SSAOrboDepth);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT,
                                new_size.x, new_size.y);

          glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_size.x, new_size.y, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, NULL);
          glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_size.x, new_size.y, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, NULL);

          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloFinalSceneSSAO scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <iostream>
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
class HelloFinalScene final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void renderScene(Pipeline& shader);

 private:
  const std::string_view textVertexShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr_with_shadow_multiplelight.vert";
  const std::string_view textFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_model/pbr_with_shadow_multiplelight.frag";

  Pipeline pbr_pipeline;
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
      glm::vec3(-1.6f, -0.6f, -2.6f),
  };

  glm::vec3 directionalLightPosition = glm::vec3(-1.0f, 1.0f, 0.0f);
  glm::vec3 directionalLightDirection =
      glm::normalize(glm::vec3(-4.0f) - directionalLightPosition);

  glm::vec3 lightColors[4] = {
      glm::vec3(0.0f, 2.8f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

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

  const std::string_view michelle_model_path = "data/model/Flower/Flower.obj";
  Model michelle_;

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

  const std::string_view trunk_model_path = "data/model/Tree/trunk.obj";
  Model trunk_;
  const std::string_view leaves_model_path = "data/model/Tree/leaves.obj";
  Model leaves_;

  Texture leavesTexture;
  const std::string_view leavesTextureFilePath_ =
      "data/model/Tree/willow_leaves.png";

  Texture TrunkAlbedo;
  const std::string_view trunkAFilePath_ = "data/model/Tree/trunk_Albedo.jpg";

  Texture TrunkNormal;
  const std::string_view trunkNFilePath_ = "data/model/Tree/trunk_Normal.jpg";

  Texture TrunkRoughness;
  const std::string_view trunkRFilePath_ =
      "data/model/Tree/trunk_Roughness.jpg";

  Texture TrunkAO;
  const std::string_view trunkAOFilePath_ = "data/model/Tree/trunk_AO.jpg";

  Pipeline blending;
  const std::string_view blendingVertexShaderFilePath_ =
      "data/shaders/hello_model/model.vert";
  const std::string_view blendingFragmentShaderFilePath_ =
      "data/shaders/hello_model/model.frag";

  Texture lanternAlbedo;
  const std::string_view lanternAFilePath_ =
      "data/model/lantern/lantern_BaseColor.png";

  Texture lanternNormal;
  const std::string_view lanternNFilePath_ =
      "data/model/lantern/lantern_Normal.png";

  Texture lanternRoughness;
  const std::string_view lanternRFilePath_ =
      "data/model/lantern/lantern_Roughness.png";

  Texture lanternMetallic;
  const std::string_view lanternMFilePath_ =
      "data/model/lantern/lantern_Metallic.png";

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

  Pipeline screen_pipeline;

  const std::string_view grass2dVertexShaderFilePath_ =
      "data/shaders/hello_IBL_model/grass_shadow.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_model/grass_shadow.frag";
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
      "data/shaders/hello_phys_bloom/bloom_final.frag";
  Pipeline shaderBloomFinal;
};

void HelloFinalScene::Begin() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  // quad_.SetUpQuad();
  cube_.SetUpCube();
  sphere_.SetUpSphere();
  lightCube_.SetUpCubeOpenGL();
  plane_.SetUpPlane();
  bloomquad_.SetUpQuadbrdf();

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

  simpleDepthShader.CreateProgram(shadow_mapping_depthVertexShaderFilePath_,
                                  shadow_mapping_depthFragmentShaderFilePath_);

  shaderLight.CreateProgram(light_boxVertexShaderFilePath_,
                            light_boxFragmentShaderFilePath_);

  blending.CreateProgram(blendingVertexShaderFilePath_,
                         blendingFragmentShaderFilePath_);

  simpleDepthInstantiateShader.CreateProgram(
      instantiate_shadow_mapping_depthVertexShaderFilePath_,
      shadow_mapping_depthFragmentShaderFilePath_);

  shadowmap_.SetUp();

  cat_.loadModel(cat_model_path.data());
  michelle_.loadModel(michelle_model_path.data());
  table_.loadModel(table_model_path.data());
  lantern_.loadModel(lantern_model_path.data());
  lantern_part2_.loadModel(lantern_part2_model_path.data());
  tea_.loadModel(tea_model_path.data());
  leaves_.loadModel(leaves_model_path.data());
  trunk_.loadModel(trunk_model_path.data());

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
  leavesTexture.is_uv_inverted = false;
  leavesTexture.TextureFromFile(leavesTextureFilePath_.data());

  TrunkAlbedo.is_uv_inverted = false;
  TrunkAlbedo.TextureFromFile(trunkAFilePath_.data());
  TrunkAO.is_uv_inverted = false;
  TrunkAO.TextureFromFile(trunkAOFilePath_.data());
  TrunkNormal.is_uv_inverted = false;
  TrunkNormal.TextureFromFile(trunkNFilePath_.data());
  TrunkRoughness.is_uv_inverted = false;
  TrunkRoughness.TextureFromFile(trunkRFilePath_.data());

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

  TableAlbedo.is_uv_inverted = false;
  TableAlbedo.TextureFromFile(tableAFilePath_.data());
  TableNormal.is_uv_inverted = false;
  TableNormal.TextureFromFile(tableNFilePath_.data());
  TableMetallic.is_uv_inverted = false;
  TableMetallic.TextureFromFile(tableMFilePath_.data());
  TableRoughness.is_uv_inverted = false;
  TableRoughness.TextureFromFile(tableRFilePath_.data());
  TableAO.is_uv_inverted = false;
  TableAO.TextureFromFile(tableAOFilePath_.data());

  lanternAlbedo.is_uv_inverted = false;
  lanternAlbedo.TextureFromFile(lanternAFilePath_.data());
  lanternNormal.is_uv_inverted = false;
  lanternNormal.TextureFromFile(lanternNFilePath_.data());
  lanternMetallic.is_uv_inverted = false;
  lanternMetallic.TextureFromFile(lanternMFilePath_.data());
  lanternRoughness.is_uv_inverted = false;
  lanternRoughness.TextureFromFile(lanternRFilePath_.data());

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
  pbr_pipeline.SetInt("shadowMap", 10);

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

  pbr_pipeline.SetVec3("lightPositions[0]", lightPositions[0]);
  pbr_pipeline.SetVec3("lightColors[0]", lightColors[0]);

  pbr_pipeline.SetVec3("directionalLightDirection", directionalLightDirection);
  pbr_pipeline.SetVec3("directionalLightColor", glm::vec3(1.0f));

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
}

void HelloFinalScene::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

void HelloFinalScene::renderScene(Pipeline& shader) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.6f, -0.8f, -1.8f));
  model = glm::rotate(model, 3.14f, glm::vec3(0.0f, -1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(3.4f));
  shader.SetMat4("model", model);
  michelle_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(1.4f, -0.92f, -2.4f));
  model = glm::rotate(model, 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.2f));
  shader.SetMat4("model", model);
  cat_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -2.2f, -2.2f));
  model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.6f));
  model = glm::rotate(model, 1.58f, glm::vec3(0.0f, 1.0f, 0.0f));
  shader.SetMat4("model", model);
  table_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.6f, -0.52f, -2.6f));
  model = glm::scale(model, glm::vec3(2.0f));
  shader.SetMat4("model", model);
  lantern_.Draw(shader.program_);
  lantern_part2_.Draw(shader.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-0.6f, -0.9f, -1.8f));
  model = glm::scale(model, glm::vec3(2.8f));
  shader.SetMat4("model", model);
  tea_.Draw(shader.program_);
}

void HelloFinalScene::Update(float dt) {
  // colorBuffer.Reset();
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  // render
  // ------
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shadowmap_.GenerateShadowMap(directionalLightPosition,
                               directionalLightDirection);
  simpleDepthShader.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  renderScene(simpleDepthShader);

  //// INSTANTIATE ELEMENTS
  //simpleDepthInstantiateShader.SetMat4("lightSpaceMatrix",shadowmap_.lightSpaceMatrix);
  //grass_.Render();
  shadowmap_.Reset();

  pbr_pipeline.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  pbr_pipeline.SetVec3("viewPos", camera_.position_);
  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, shadowmap_.depthMap);

  // pbr_pipeline.SetInt("shadowMap", 10);

  // 1. render scene into floating point framebuffer
  // -----------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
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
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));

  AluAlbedo.BindTexture(GL_TEXTURE3);
  AluNormal.BindTexture(GL_TEXTURE4);
  AluMetallic.BindTexture(GL_TEXTURE5);
  AluRoughness.BindTexture(GL_TEXTURE6);
  AluMetallic.BindTexture(GL_TEXTURE7);

  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, shadowmap_.depthMap);
  pbr_pipeline.SetInt("shadowMap", 10);
  grass2d_pipeline.SetInt("shadowMap", 10);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.6f, -0.8f, -1.8f));
  model = glm::rotate(model, 3.14f, glm::vec3(0.0f, -1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(3.4f));
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
  michelle_.Draw(pbr_pipeline.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-0.6f, -0.9f, -1.8f));
  model = glm::scale(model, glm::vec3(2.8f));
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
  tea_.Draw(pbr_pipeline.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(1.4f, -0.92f, -2.4f));
  model = glm::rotate(model, 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.2f));

  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
  pbr_pipeline.SetMat4("model", model);

  catBaseColor.BindTexture(GL_TEXTURE3);
  catNormal.BindTexture(GL_TEXTURE4);
  catMetallic.BindTexture(GL_TEXTURE5);
  catRoughness.BindTexture(GL_TEXTURE6);
  catAo.BindTexture(GL_TEXTURE7);

  cat_.Draw(pbr_pipeline.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -2.2f, -2.2f));
  model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.6f));
  model = glm::rotate(model, 1.58f, glm::vec3(0.0f, 1.0f, 0.0f));
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
  TableAlbedo.BindTexture(GL_TEXTURE3);
  TableNormal.BindTexture(GL_TEXTURE4);
  TableMetallic.BindTexture(GL_TEXTURE5);
  TableRoughness.BindTexture(GL_TEXTURE6);
  TableAO.BindTexture(GL_TEXTURE7);
  table_.Draw(pbr_pipeline.program_);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(lightPositions[0]));
  model = glm::scale(model, glm::vec3(0.2f));
  model = glm::scale(model, glm::vec3(0.4f, 1.0f, 0.4f));
  shaderLight.SetMat4("model", model);

  shaderLight.SetVec3("lightColor", lightColors[0]);
  shaderLight.SetMat4("projection", projection);
  shaderLight.SetMat4("view", view);
  // lightCube_.RenderCubeOpenGL();
  sphere_.RenderSphere();

  // Disable face culling for 2d and skybox and lantern because of transparancy
  glDisable(GL_CULL_FACE);

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.6f, -0.52f, -2.6f));
  model = glm::scale(model, glm::vec3(2.0f));
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
  lanternAlbedo.BindTexture(GL_TEXTURE3);
  lanternNormal.BindTexture(GL_TEXTURE4);
  lanternMetallic.BindTexture(GL_TEXTURE5);
  lanternRoughness.BindTexture(GL_TEXTURE6);
  lantern_.Draw(pbr_pipeline.program_);
  AluAlbedo.BindTexture(GL_TEXTURE3);
  AluNormal.BindTexture(GL_TEXTURE4);
  AluMetallic.BindTexture(GL_TEXTURE5);
  AluRoughness.BindTexture(GL_TEXTURE6);
  AluMetallic.BindTexture(GL_TEXTURE7);
  lantern_part2_.Draw(pbr_pipeline.program_);

  model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(0.4f));
  model = glm::translate(model, glm::vec3(1.0f, -4.0f, 0.0f));
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));

  meaAlbedo.BindTexture(GL_TEXTURE3);
  meaNormal.BindTexture(GL_TEXTURE4);
  meaMetallic.BindTexture(GL_TEXTURE5);
  meaRoughness.BindTexture(GL_TEXTURE6);
  meaAO.BindTexture(GL_TEXTURE7);

  plane_.RenderPlane();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-2.2f, -1.8f, -8.2f));
  model = glm::rotate(model, 2.1f, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(2.8f));
  pbr_pipeline.SetMat4("model", model);
  pbr_pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
  TrunkAlbedo.BindTexture(GL_TEXTURE3);
  TrunkNormal.BindTexture(GL_TEXTURE4);
  TrunkRoughness.BindTexture(GL_TEXTURE6);
  TrunkAO.BindTexture(GL_TEXTURE7);
  trunk_.Draw(pbr_pipeline.program_);

  blending.SetMat4("model", model);
  blending.SetMat4("view", view);
  blending.SetMat4("projection", projection);
  leavesTexture.BindTexture(GL_TEXTURE8);
  blending.SetInt("texture_diffuse1", 8);
  leaves_.Draw(blending.program_);

  float dimtime = 2 * timer_;
  float dimwindStrength = 0.18f;  // Adjust as needed
  model = glm::mat4(1.0f);
  grass2d_pipeline.SetFloat("time", dimtime);
  grass2d_pipeline.SetFloat("windStrength", dimwindStrength);
  grass_texture_.BindTexture(GL_TEXTURE8);
  grass2d_pipeline.SetInt("texture1", 8);
  model = glm::scale(model, glm::vec3(8, 8, 8));
  grass2d_pipeline.SetMat4("model", model);
  grass2d_pipeline.SetMat4("view", view);
  grass2d_pipeline.SetMat4("projection", projection);
  grass2d_pipeline.SetVec3("viewPos", camera_.position_);
  grass2d_pipeline.SetVec3("lightPos", directionalLightPosition);
  grass2d_pipeline.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  grass2d_pipeline.SetVec3("aNormal", glm::vec3(0.0f, 1.0f, 0.0f));
  grass_.Render();

  // render skybox (render as last to prevent overdraw)
  backgroundShader.SetMat4("view", view);
  cubemapsHDR_.DrawHDR();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if (programChoice < 1 || programChoice > 3) {
    programChoice = 1;
  }
  bloom = (programChoice == 1) ? false : true;
  bool horizontal = true;

  //  use unthresholded bloom with progressive downsample/upsampling
  // -------------------------------------------------------------------

  bloomRenderer.RenderBloomTexture(colorBuffers[1], bloomFilterRadius);

  // 3. now render floating point color buffer to 2D quad and tonemap HDR colors
  // to default framebuffer's (clamped) color range
  // --------------------------------------------------------------------------------------------------------------------------
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shaderBloomFinal.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
  glActiveTexture(GL_TEXTURE1);
  if (programChoice == 1) {
    glBindTexture(
        GL_TEXTURE_2D,
        0);  // trick to bind invalid texture "0", we don't care either way!
  }
  if (programChoice == 2) {
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
  } else if (programChoice == 3) {
    glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());
  }
  shaderBloomFinal.SetInt("programChoice", programChoice);
  shaderBloomFinal.SetFloat("exposure", exposure);
  bloomquad_.RenderQuadbrdf();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloFinalScene scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
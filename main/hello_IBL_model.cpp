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
class HelloIBLModel final : public Scene {
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
      "data/shaders/hello_IBL/cubemap_hdr.vert";
  const std::string_view equirectangleFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/cubemap_hdr.frag";

  Pipeline equirectangular_to_cubemap_pipeline;

  const std::string_view backgroundVertexShaderFilePath_ =
      "data/shaders/hello_IBL/background.vert";
  const std::string_view backgroundFragmentShaderFilePath_ =
      "data/shaders/hello_IBL/background.frag";

  Pipeline backgroundShader;

  unsigned int captureFBO;
  unsigned int captureRBO;

  Texture hdr_cubemap;
  const std::string_view hdr_cubemapFilePath_ =
      "data/textures/xiequ_yuan_hdr/xiequ_yuan.hdr";

  // const std::string_view hdr_cubemapFilePath_ =
  //    "data/textures/newport_hdr/newport.hdr";

  // unsigned int envCubemap;

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

  PrimitiveObjects quad_;
  PrimitiveObjects cube_;
  PrimitiveObjects sphere_;

  Cubemaps cubemapsHDR_;
};

void HelloIBLModel::Begin() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  quad_.SetUpQuad();
  cube_.SetUpCube();
  sphere_.SetUpSphere();

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

  albedo.BindTexture(GL_TEXTURE3);
  normal.BindTexture(GL_TEXTURE4);
  metallic.BindTexture(GL_TEXTURE5);
  roughness.BindTexture(GL_TEXTURE6);
  ao.BindTexture(GL_TEXTURE7);

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

  //// pbr: setup framebuffer
  //// ----------------------

  // glGenFramebuffers(1, &captureFBO);
  // glGenRenderbuffers(1, &captureRBO);

  // glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  // glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 4096, 4096);
  // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
  //                           GL_RENDERBUFFER, captureRBO);

  //// pbr: setup cubemap to render to and attach to framebuffer
  //// ---------------------------------------------------------
  // glGenTextures(1, &envCubemap);
  // glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  // for (unsigned int i = 0; i < 6; ++i) {
  //   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 4096,
  //   4096,
  //                0, GL_RGB, GL_FLOAT, nullptr);
  // }
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // glTexParameteri(
  //     GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
  //     GL_LINEAR_MIPMAP_LINEAR);  // enable pre-filter mipmap sampling
  //                                // (combatting visible dots artifact)
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //// pbr: set up projection and view matrices for capturing data onto the 6
  //// cubemap face directions
  ////
  ///----------------------------------------------------------------------------------------------
  // glm::mat4 captureProjection =
  //     glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  // glm::mat4 captureViews[] = {
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
  //                 glm::vec3(0.0f, -1.0f, 0.0f)),
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
  //                 glm::vec3(0.0f, -1.0f, 0.0f)),
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
  //                 glm::vec3(0.0f, 0.0f, 1.0f)),
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
  //                 glm::vec3(0.0f, 0.0f, -1.0f)),
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
  //                 glm::vec3(0.0f, -1.0f, 0.0f)),
  //     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  //                 glm::vec3(0.0f, -1.0f, 0.0f))};

  //// pbr: convert HDR equirectangular environment map to cubemap equivalent
  //// ----------------------------------------------------------------------

  // equirectangular_to_cubemap_pipeline.SetInt("equirectangularMap", 0);
  // equirectangular_to_cubemap_pipeline.SetMat4("projection",
  // captureProjection); hdr_cubemap.BindTextureHDR(GL_TEXTURE0);

  // glViewport(0, 0, 4096, 4096);  // don't forget to configure the viewport to
  //                                // the
  //                                // capture dimensions.
  // glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  // for (unsigned int i = 0; i < 6; ++i) {
  //   equirectangular_to_cubemap_pipeline.SetMat4("view", captureViews[i]);
  //   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
  //                          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap,
  //                          0);
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //  cube_.RenderCube();
  //}
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //// then let OpenGL generate mipmaps from first mip face (combatting visible
  //// dots artifact)
  // glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  //// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance
  //// scale.
  ////
  ///--------------------------------------------------------------------------------
  // glGenTextures(1, &irradianceMap);
  // glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
  // for (unsigned int i = 0; i < 6; ++i) {
  //   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
  //                GL_RGB, GL_FLOAT, nullptr);
  // }
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  // glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

  //// pbr: solve diffuse integral by convolution to create an irradiance
  //// (cube)map.
  ////
  ///-----------------------------------------------------------------------------
  // irradianceShader.SetInt("environmentMap", 0);
  // irradianceShader.SetMat4("projection", captureProjection);
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  // glViewport(
  //     0, 0, 32,
  //     32);  // don't forget to configure the viewport to the capture
  //     dimensions.
  // glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  // for (unsigned int i = 0; i < 6; ++i) {
  //   irradianceShader.SetMat4("view", captureViews[i]);
  //   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
  //                          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap,
  //                          0);
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //  cube_.RenderCube();
  //}
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter
  // scale.
  // --------------------------------------------------------------------------------
  // glGenTextures(1, &prefilterMap);
  // glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
  // for (unsigned int i = 0; i < 6; ++i) {
  //  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128,
  //  0,
  //               GL_RGB, GL_FLOAT, nullptr);
  //}
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
  //                GL_LINEAR_MIPMAP_LINEAR);  // be sure to set minification
  //                                           // filter to mip_linear
  // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //// generate mipmaps for the cubemap so OpenGL automatically allocates the
  //// required memory.
  // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  //// pbr: run a quasi monte-carlo simulation on the environment lighting to
  //// create a prefilter (cube)map.
  ////
  ///----------------------------------------------------------------------------------------------------
  // prefilterShader.SetInt("environmentMap", 0);
  // prefilterShader.SetMat4("projection", captureProjection);
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  // glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  // unsigned int maxMipLevels = 5;
  // for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
  //   // reisze framebuffer according to mip-level size.
  //   unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5,
  //   mip)); unsigned int mipHeight =
  //       static_cast<unsigned int>(128 * std::pow(0.5, mip));
  //   glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  //   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
  //                         mipHeight);
  //   glViewport(0, 0, mipWidth, mipHeight);

  //  float roughness = (float)mip / (float)(maxMipLevels - 1);
  //  prefilterShader.SetFloat("roughness", roughness);
  //  for (unsigned int i = 0; i < 6; ++i) {
  //    prefilterShader.SetMat4("view", captureViews[i]);
  //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
  //                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap,
  //                           mip);

  //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //    cube_.RenderCube();
  //  }
  //}
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //// pbr: generate a 2D LUT from the BRDF equations used.
  //// ----------------------------------------------------
  // glGenTextures(1, &brdfLUTTexture);

  //// pre-allocate enough memory for the LUT texture.
  // glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
  //// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //// then re-configure capture framebuffer object and render screen-space
  /// quad_ / with BRDF shader.
  // glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  // glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
  //                        brdfLUTTexture, 0);

  // glViewport(0, 0, 512, 512);
  // brdfShader.Use();
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // quad_.RenderQuad();

  // glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // initialize static shader uniforms before rendering
  // --------------------------------------------------
  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  pbr_pipeline.SetMat4("projection", projection);
  backgroundShader.SetMat4("projection", projection);

  glViewport(0, 0, 1280, 720);
}

void HelloIBLModel::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

void HelloIBLModel::Update(float dt) {
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  // render
  // ------
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  // render skybox (render as last to prevent overdraw)
  backgroundShader.SetMat4("view", view);
  cubemapsHDR_.DrawHDR();

  // render BRDF map to screen
  // brdfShader.Use();
  // renderquad_();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloIBLModel scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
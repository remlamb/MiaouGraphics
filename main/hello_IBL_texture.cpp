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
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloIBLTexture final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view textVertexShaderFilePath_ =
      "data/shaders/hello_IBL_texture/pbr.vert";
  const std::string_view textFragmentShaderFilePath_ =
      "data/shaders/hello_IBL_texture/pbr.frag";

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

  unsigned int envCubemap;

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

  unsigned int brdfLUTTexture;
  unsigned int irradianceMap;
  unsigned int prefilterMap;
};

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube() {
  // initialize (if necessary)
  if (cubeVAO == 0) {
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,    // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,   // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,    // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,    // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,   // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,    // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,    // top-right
                                                             // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,      // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,    // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,     // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,    // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,      // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,     // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,   // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,    // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,    // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,    // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,    // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f    // bottom-left
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // render Cube
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void HelloIBLTexture::Begin() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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

  albedo.TextureFromFile(albedoFilePath_.data());
  normal.TextureFromFile(normalFilePath_.data());
  metallic.TextureFromFile(metallicFilePath_.data());
  roughness.TextureFromFile(roughnessFilePath_.data());
  ao.TextureFromFile(aoFilePath_.data());

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

  // pbr: setup framebuffer
  // ----------------------

  glGenFramebuffers(1, &captureFBO);
  glGenRenderbuffers(1, &captureRBO);

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 4096, 4096);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, captureRBO);

  // pbr: setup cubemap to render to and attach to framebuffer
  // ---------------------------------------------------------
  glGenTextures(1, &envCubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 4096, 4096,
                 0, GL_RGB, GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(
      GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
      GL_LINEAR_MIPMAP_LINEAR);  // enable pre-filter mipmap sampling
                                 // (combatting visible dots artifact)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // pbr: set up projection and view matrices for capturing data onto the 6
  // cubemap face directions
  // ----------------------------------------------------------------------------------------------
  glm::mat4 captureProjection =
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  glm::mat4 captureViews[] = {
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f))};

  // pbr: convert HDR equirectangular environment map to cubemap equivalent
  // ----------------------------------------------------------------------

  equirectangular_to_cubemap_pipeline.SetInt("equirectangularMap", 0);
  equirectangular_to_cubemap_pipeline.SetMat4("projection", captureProjection);
  hdr_cubemap.BindTextureHDR(GL_TEXTURE0);

  glViewport(0, 0, 4096, 4096);  // don't forget to configure the viewport to
                                 // the
                                 // capture dimensions.
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  for (unsigned int i = 0; i < 6; ++i) {
    equirectangular_to_cubemap_pipeline.SetMat4("view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderCube();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // then let OpenGL generate mipmaps from first mip face (combatting visible
  // dots artifact)
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance
  // scale.
  // --------------------------------------------------------------------------------
  glGenTextures(1, &irradianceMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
                 GL_RGB, GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

  // pbr: solve diffuse integral by convolution to create an irradiance
  // (cube)map.
  // -----------------------------------------------------------------------------
  irradianceShader.SetInt("environmentMap", 0);
  irradianceShader.SetMat4("projection", captureProjection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  glViewport(
      0, 0, 32,
      32);  // don't forget to configure the viewport to the capture dimensions.
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  for (unsigned int i = 0; i < 6; ++i) {
    irradianceShader.SetMat4("view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap,
                           0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderCube();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter
  // scale.
  // --------------------------------------------------------------------------------
  glGenTextures(1, &prefilterMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,
                 GL_RGB, GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);  // be sure to set minification
                                             // filter to mip_linear
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // generate mipmaps for the cubemap so OpenGL automatically allocates the
  // required memory.
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  // pbr: run a quasi monte-carlo simulation on the environment lighting to
  // create a prefilter (cube)map.
  // ----------------------------------------------------------------------------------------------------
  prefilterShader.SetInt("environmentMap", 0);
  prefilterShader.SetMat4("projection", captureProjection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  unsigned int maxMipLevels = 5;
  for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
    // reisze framebuffer according to mip-level size.
    unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
    unsigned int mipHeight =
        static_cast<unsigned int>(128 * std::pow(0.5, mip));
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
                          mipHeight);
    glViewport(0, 0, mipWidth, mipHeight);

    float roughness = (float)mip / (float)(maxMipLevels - 1);
    prefilterShader.SetFloat("roughness", roughness);
    for (unsigned int i = 0; i < 6; ++i) {
      prefilterShader.SetMat4("view", captureViews[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap,
                             mip);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      renderCube();
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // pbr: generate a 2D LUT from the BRDF equations used.
  // ----------------------------------------------------
  glGenTextures(1, &brdfLUTTexture);

  // pre-allocate enough memory for the LUT texture.
  glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
  // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // then re-configure capture framebuffer object and render screen-space quad
  // with BRDF shader.
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         brdfLUTTexture, 0);

  glViewport(0, 0, 512, 512);
  brdfShader.Use();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  renderQuad();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // initialize static shader uniforms before rendering
  // --------------------------------------------------
  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  pbr_pipeline.SetMat4("projection", projection);
  backgroundShader.SetMat4("projection", projection);

  glViewport(0, 0, 1280, 720);
}

void HelloIBLTexture::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere() {
  if (sphereVAO == 0) {
    glGenVertexArrays(1, &sphereVAO);

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
      if (!oddRow)  // even rows: y == 0, y == 2; and so on
      {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      } else {
        for (int x = X_SEGMENTS; x >= 0; --x) {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0) {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0) {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)(6 * sizeof(float)));
  }

  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void HelloIBLTexture::Update(float dt) {
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
  glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

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
      renderSphere();
    }
  }

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
    renderSphere();
  }

  // render skybox (render as last to prevent overdraw)
  backgroundShader.SetMat4("view", view);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  // glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance
  // map glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter
  // map
  renderCube();

  // render BRDF map to screen
  // brdfShader.Use();
  // renderQuad();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloIBLTexture scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
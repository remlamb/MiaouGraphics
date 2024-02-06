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

struct bloomMip {
  glm::vec2 size;
  glm::ivec2 intSize;
  unsigned int texture;
};

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
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

class bloomFBO {
 public:
  bloomFBO();
  ~bloomFBO();
  bool Init(unsigned int windowWidth, unsigned int windowHeight,
            unsigned int mipChainLength);
  void Destroy();
  void BindForWriting();
  const std::vector<bloomMip>& MipChain() const;

 private:
  bool mInit;
  unsigned int mFBO;
  std::vector<bloomMip> mMipChain;
};

bloomFBO::bloomFBO() : mInit(false) {}
bloomFBO::~bloomFBO() {}

bool bloomFBO::Init(unsigned int windowWidth, unsigned int windowHeight,
                    unsigned int mipChainLength) {
  if (mInit) return true;

  glGenFramebuffers(1, &mFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

  glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
  glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);
  // Safety check
  if (windowWidth > (unsigned int)INT_MAX ||
      windowHeight > (unsigned int)INT_MAX) {
    std::cerr << "Window size conversion overflow - cannot build bloom FBO!"
              << std::endl;
    return false;
  }

  mMipChain.reserve(mipChainLength);

  for (unsigned int i = 0; i < mipChainLength; i++) {
    bloomMip mip;

    mipSize *= 0.5f;
    mipIntSize /= 2;
    mip.size = mipSize;
    mip.intSize = mipIntSize;

    glGenTextures(1, &mip.texture);
    glBindTexture(GL_TEXTURE_2D, mip.texture);
    // we are downscaling an HDR color buffer, so we need a float texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x,
                 (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y
              << std::endl;
    mMipChain.emplace_back(mip);
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         mMipChain[0].texture, 0);

  // setup attachments
  unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, attachments);

  // check completion status
  int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    printf("gbuffer FBO error, status: 0x%x\n", status);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  mInit = true;
  return true;
}

void bloomFBO::Destroy() {
  for (int i = 0; i < (int)mMipChain.size(); i++) {
    glDeleteTextures(1, &mMipChain[i].texture);
    mMipChain[i].texture = 0;
  }
  glDeleteFramebuffers(1, &mFBO);
  mFBO = 0;
  mInit = false;
}

void bloomFBO::BindForWriting() { glBindFramebuffer(GL_FRAMEBUFFER, mFBO); }

const std::vector<bloomMip>& bloomFBO::MipChain() const { return mMipChain; }

class BloomRenderer {
 public:
  BloomRenderer();
  ~BloomRenderer();
  bool Init(unsigned int windowWidth, unsigned int windowHeight);
  void Destroy();
  void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
  unsigned int BloomTexture();
  unsigned int BloomMip_i(int index);
  Pipeline mDownsampleShader;
  Pipeline mUpsampleShader;

 private:
  void RenderDownsamples(unsigned int srcTexture);
  void RenderUpsamples(float filterRadius);

  bool mInit;
  bloomFBO mFBO;
  glm::ivec2 mSrcViewportSize;
  glm::vec2 mSrcViewportSizeFloat;

  const std::string_view upVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/new_upsample.vert";
  const std::string_view upFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/new_upsample.frag";

  const std::string_view downVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/new_downsample.vert";
  const std::string_view downFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/new_downsample.frag";

  bool mKarisAverageOnDownsample = true;
};

BloomRenderer::BloomRenderer() : mInit(false) {}
BloomRenderer::~BloomRenderer() {}

bool BloomRenderer::Init(unsigned int windowWidth, unsigned int windowHeight) {
  if (mInit) return true;
  mSrcViewportSize = glm::ivec2(windowWidth, windowHeight);
  mSrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

  // Framebuffer
  const unsigned int num_bloom_mips = 6;  // TODO: Play around with this value
  bool status = mFBO.Init(windowWidth, windowHeight, num_bloom_mips);
  if (!status) {
    std::cerr
        << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
    return false;
  }

  // Shaders
  mUpsampleShader.CreateProgram(upVertexShaderFilePath_,
                                upFragmentShaderFilePath_);

  mDownsampleShader.CreateProgram(downVertexShaderFilePath_,
                                  downFragmentShaderFilePath_);
  // Downsample
  mDownsampleShader.SetInt("srcTexture", 0);
  glUseProgram(0);

  // Upsample
  mUpsampleShader.SetInt("srcTexture", 0);
  glUseProgram(0);

  return true;
}

void BloomRenderer::Destroy() {
  mFBO.Destroy();
  // delete mDownsampleShader;
  // delete mUpsampleShader;
}

void BloomRenderer::RenderDownsamples(unsigned int srcTexture) {
  const std::vector<bloomMip>& mipChain = mFBO.MipChain();

  mDownsampleShader.SetVec2("srcResolution", mSrcViewportSizeFloat);
  if (mKarisAverageOnDownsample) {
    mDownsampleShader.SetInt("mipLevel", 0);
  }

  // Bind srcTexture (HDR color buffer) as initial texture input
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, srcTexture);

  // Progressively downsample through the mip chain
  for (int i = 0; i < (int)mipChain.size(); i++) {
    const bloomMip& mip = mipChain[i];
    glViewport(0, 0, mip.size.x, mip.size.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           mip.texture, 0);

    // Render screen-filled quad of resolution of current mip
    renderQuad();

    // Set current mip resolution as srcResolution for next iteration
    mDownsampleShader.SetVec2("srcResolution", mip.size);
    // Set current mip as texture input for next iteration
    glBindTexture(GL_TEXTURE_2D, mip.texture);
    // Disable Karis average for consequent downsamples
    if (i == 0) {
      mDownsampleShader.SetInt("mipLevel", 1);
    }
  }

  glUseProgram(0);
}

void BloomRenderer::RenderUpsamples(float filterRadius) {
  const std::vector<bloomMip>& mipChain = mFBO.MipChain();

  mUpsampleShader.SetFloat("filterRadius", filterRadius);

  // Enable additive blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBlendEquation(GL_FUNC_ADD);

  for (int i = (int)mipChain.size() - 1; i > 0; i--) {
    const bloomMip& mip = mipChain[i];
    const bloomMip& nextMip = mipChain[i - 1];

    // Bind viewport and texture from where to read
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mip.texture);

    // Set framebuffer render target (we write to this texture)
    glViewport(0, 0, nextMip.size.x, nextMip.size.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           nextMip.texture, 0);

    // Render screen-filled quad of resolution of current mip
    renderQuad();
  }

  // Disable additive blending
  // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_BLEND);

  glUseProgram(0);
}

void BloomRenderer::RenderBloomTexture(unsigned int srcTexture,
                                       float filterRadius) {
  mFBO.BindForWriting();

  this->RenderDownsamples(srcTexture);
  this->RenderUpsamples(filterRadius);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // Restore viewport
  glViewport(0, 0, mSrcViewportSize.x, mSrcViewportSize.y);
}

GLuint BloomRenderer::BloomTexture() { return mFBO.MipChain()[0].texture; }

GLuint BloomRenderer::BloomMip_i(int index) {
  const std::vector<bloomMip>& mipChain = mFBO.MipChain();
  int size = (int)mipChain.size();
  return mipChain[(index > size - 1) ? size - 1
                  : (index < 0)      ? 0
                                     : index]
      .texture;
}

class HelloBloomPB final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  // settings
  bool bloom = true;
  float exposure = 1.0f;
  int programChoice = 3;
  float bloomFilterRadius = 0.005f;

  Texture wood;
  const std::string_view woodFilePath_ = "data/textures/wood_.jpg";

  const std::string_view bloomVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/bloom.vert";
  const std::string_view bloomFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/bloom.frag";
  Pipeline shader;

  const std::string_view light_boxFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/light_box.frag";
  Pipeline shaderLight;

  const std::string_view oldBlurVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/old_blur.vert";
  const std::string_view oldBlurFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/old_blur.frag";
  Pipeline shaderBlur;

  const std::string_view bloomFinalVertexShaderFilePath_ =
      "data/shaders/hello_phys_bloom/bloom_final.vert";
  const std::string_view bloomFinalFragmentShaderFilePath_ =
      "data/shaders/hello_phys_bloom/bloom_final.frag";
  Pipeline shaderBloomFinal;

  unsigned int hdrFBO;
  unsigned int colorBuffers[2];

  unsigned int rboDepth;
  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

  unsigned int pingpongFBO[2];
  unsigned int pingpongColorbuffers[2];

  BloomRenderer bloomRenderer;

  // lighting info
  // -------------
  // positions
  std::vector<glm::vec3> lightPositions;

  // colors
  std::vector<glm::vec3> lightColors;

  PrimitiveObjects quad_;
  PrimitiveObjects cube_;
  PrimitiveObjects sphere_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  float timer_;
  Camera camera_;
};

void HelloBloomPB::Begin() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  quad_.SetUpQuad();
  cube_.SetUpCube();
  sphere_.SetUpSphere();

  wood.HDRTextureFromFile(woodFilePath_);

  shader.CreateProgram(bloomVertexShaderFilePath_,
                       bloomFragmentShaderFilePath_);

  shaderLight.CreateProgram(bloomVertexShaderFilePath_,
                            light_boxFragmentShaderFilePath_);

  shaderBlur.CreateProgram(oldBlurVertexShaderFilePath_,
                           oldBlurFragmentShaderFilePath_);

  shaderBloomFinal.CreateProgram(bloomFinalVertexShaderFilePath_,
                                 bloomFinalFragmentShaderFilePath_);

  lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
  lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
  lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
  lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));

  lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
  lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
  lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
  lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  glViewport(0, 0, Engine::screen_width_, Engine::screen_height_);

  // shader configuration
  // --------------------
  shader.SetInt("diffuseTexture", 0);
  shaderBlur.SetInt("image", 0);

  shaderBloomFinal.SetInt("scene", 0);
  shaderBloomFinal.SetInt("bloomBlur", 1);

  //___NEW___
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
  shader.SetInt("diffuseTexture", 0);
  shaderBlur.SetInt("image", 0);
  shaderBloomFinal.SetInt("scene", 0);
  shaderBloomFinal.SetInt("bloomBlur", 1);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // bloom renderer
  // --------------
  bloomRenderer.mDownsampleShader.SetInt("srcTexture", 0);
  bloomRenderer.mUpsampleShader.SetInt("srcTexture", 0);
  bloomRenderer.Init(Engine::screen_width_, Engine::screen_height_);
}

void HelloBloomPB::End() {
  // Unload program/pipeline
  // cubemaps_.Delete();
}

void HelloBloomPB::Update(float dt) {
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  //Switch Type
  const Uint8* keys = SDL_GetKeyboardState(nullptr);
  if (keys[SDL_SCANCODE_0]) {
    programChoice = 1;
  }
  if (keys[SDL_SCANCODE_8]) {
    programChoice = 2;
  }
  if (keys[SDL_SCANCODE_9]) {
    programChoice = 3;
  }



  // render
  // ------
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 1. render scene into floating point framebuffer
  // -----------------------------------------------
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  shader.SetMat4("projection", projection);
  shader.SetMat4("view", view);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, wood.hdrTexture);
  // set lighting uniforms
  for (unsigned int i = 0; i < lightPositions.size(); i++) {
    shader.SetVec3("lights[" + std::to_string(i) + "].Position",
                   lightPositions[i]);
    shader.SetVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
  }
  shader.SetVec3("viewPos", camera_.position_);
  // create one large cube that acts as the floor
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
  model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
  shader.SetMat4("model", model);
  renderCube();
  // then create multiple cubes as the scenery
  glBindTexture(GL_TEXTURE_2D, wood.hdrTexture);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.SetMat4("model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.SetMat4("model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
  model = glm::rotate(model, glm::radians(60.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  shader.SetMat4("model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
  model = glm::rotate(model, glm::radians(23.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  model = glm::scale(model, glm::vec3(1.25));
  shader.SetMat4("model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
  model = glm::rotate(model, glm::radians(124.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  shader.SetMat4("model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.SetMat4("model", model);
  renderCube();

  // finally show all the light sources as bright cubes
  shaderLight.SetMat4("projection", projection);
  shaderLight.SetMat4("view", view);

  for (unsigned int i = 0; i < lightPositions.size(); i++) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(lightPositions[i]));
    model = glm::scale(model, glm::vec3(0.25f));
    shaderLight.SetMat4("model", model);
    shaderLight.SetVec3("lightColor", lightColors[i]);
    renderCube();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if (programChoice < 1 || programChoice > 3) {
    programChoice = 1;
  }
  bloom = (programChoice == 1) ? false : true;
  bool horizontal = true;

  // 2.A) bloom is disabled
  // ----------------------
  if (programChoice == 1) {
  }

  // 2.B) blur bright fragments with two-pass Gaussian Blur
  // ------------------------------------------------------
  else if (programChoice == 2) {
    bool first_iteration = true;
    unsigned int amount = 10;
    shaderBlur.Use();
    for (unsigned int i = 0; i < amount; i++) {
      glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
      shaderBlur.SetInt("horizontal", horizontal);
      glBindTexture(
          GL_TEXTURE_2D,
          first_iteration
              ? colorBuffers[1]
              : pingpongColorbuffers[!horizontal]);  // bind texture of other
                                                     // framebuffer (or scene if
                                                     // first iteration)
      renderQuad();
      horizontal = !horizontal;
      if (first_iteration) first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  // 2.C) use unthresholded bloom with progressive downsample/upsampling
  // -------------------------------------------------------------------
  else if (programChoice == 3) {
    bloomRenderer.RenderBloomTexture(colorBuffers[1], bloomFilterRadius);
  }

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
  renderQuad();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloBloomPB scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
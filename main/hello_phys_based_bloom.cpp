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
#include "Model.h"
#include "PrimitiveObjects.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
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

  quad_.SetUpQuadbrdf();
  cube_.SetUpCubeOpenGL();

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

  // Switch Type
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
  cube_.RenderCubeOpenGL();
  // then create multiple cubes as the scenery
  glBindTexture(GL_TEXTURE_2D, wood.hdrTexture);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.SetMat4("model", model);
  cube_.RenderCubeOpenGL();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.SetMat4("model", model);
  cube_.RenderCubeOpenGL();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
  model = glm::rotate(model, glm::radians(60.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  shader.SetMat4("model", model);
  cube_.RenderCubeOpenGL();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
  model = glm::rotate(model, glm::radians(23.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  model = glm::scale(model, glm::vec3(1.25));
  shader.SetMat4("model", model);
  cube_.RenderCubeOpenGL();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
  model = glm::rotate(model, glm::radians(124.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  shader.SetMat4("model", model);
  cube_.RenderCubeOpenGL();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.SetMat4("model", model);
  cube_.RenderCubeOpenGL();

  // finally show all the light sources as bright cubes
  shaderLight.SetMat4("projection", projection);
  shaderLight.SetMat4("view", view);

  for (unsigned int i = 0; i < lightPositions.size(); i++) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(lightPositions[i]));
    model = glm::scale(model, glm::vec3(0.25f));
    shaderLight.SetMat4("model", model);
    shaderLight.SetVec3("lightColor", lightColors[i]);
    cube_.RenderCubeOpenGL();
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
      quad_.RenderQuadbrdf();
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
  quad_.RenderQuadbrdf();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloBloomPB scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
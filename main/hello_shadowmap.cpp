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
class HelloShadowmap final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void renderScene(Pipeline& shader);

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

  GLuint plane_vao_ = 0;
  GLuint plane_vbo_ = 0;
  float planeVertices[48] = {
      // positions            // normals         // texcoords
      25.0f,  -0.5f, 25.0f,  0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
      -25.0f, -0.5f, 25.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
      -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,

      25.0f,  -0.5f, 25.0f,  0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
      -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,
      25.0f,  -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f};
  Texture wood_texture_;
  const std::string_view wood_texture_FilePath_ = "data/textures/deep2.png";

  const std::string_view grass2dVertexShaderFilePath_ =
      "data/shaders/hello_model/moving_grass2d.vert";
  const std::string_view grass2dFragmentShaderFilePath_ =
      "data/shaders/hello_model/moving_grass2d.frag";
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
  glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  const std::string_view DepthShaderVertexShaderFilePath_ =
      "data/shaders/hello_shadowmap/DepthShader.vert";
  const std::string_view DepthShaderFragmentShaderFilePath_ =
      "data/shaders/hello_shadowmap/DepthShader.frag";

  Pipeline DepthShader;

  const std::string_view DepthQuadVertexShaderFilePath_ =
      "data/shaders/hello_shadowmap/DepthQuad.vert";
  const std::string_view DepthQuadFragmentShaderFilePath_ =
      "data/shaders/hello_shadowmap/DepthQuad.frag";

  Pipeline DepthQuad;

  // create depth texture
  unsigned int depthMap;

  unsigned int depthMapFBO;
};

void HelloShadowmap::Begin() {
  cubemaps_.SetUp(faces);
  cubemaps_pipeline.CreateProgram(cubemapsVertexShaderFilePath_,
                                  cubemapsFragmentShaderFilePath_);

  DepthShader.CreateProgram(DepthShaderVertexShaderFilePath_,
                            DepthShaderFragmentShaderFilePath_);

  DepthQuad.CreateProgram(DepthQuadVertexShaderFilePath_,
                          DepthQuadFragmentShaderFilePath_);

  cubemaps_pipeline.SetInt("skybox", 0);

  model_pipeline.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);

  wood_texture_.is_uv_inverted = false;
  wood_texture_.TextureFromFile(wood_texture_FilePath_);
  wood_texture_.BindTexture(GL_TEXTURE0);

  ourModel_.isInverted();
  ourModel_.loadModel(model_path.data());

  cat_.loadModel(cat_model_path.data());

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  glEnable(GL_DEPTH_TEST);

  // plane VAO
  glGenVertexArrays(1, &plane_vao_);
  glGenBuffers(1, &plane_vbo_);
  glBindVertexArray(plane_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, plane_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(6 * sizeof(float)));
  glBindVertexArray(0);

  // configure depth map FBO
  // -----------------------

  glGenFramebuffers(1, &depthMapFBO);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HelloShadowmap::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
  framebuffer_.Delete();
}

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

// renders the 3D scene
// --------------------
void HelloShadowmap::renderScene(Pipeline& shader) {
  // floor
  glm::mat4 model = glm::mat4(1.0f);
  shader.SetMat4("model", model);
  glBindVertexArray(plane_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  // cubes
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
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
  model = glm::rotate(model, glm::radians(60.0f),
                      glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
  model = glm::scale(model, glm::vec3(0.25));
  shader.SetMat4("model", model);
  renderCube();
}

void HelloShadowmap::Update(float dt) {
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  wood_texture_.BindTexture(GL_TEXTURE0);
  glBindVertexArray(plane_vao_);
  float dimtime = 6 * timer_;
  float dimwindStrength = 0.1f;  // Adjust as needed
  grass2d_pipeline.SetFloat("time", dimtime);
  grass2d_pipeline.SetFloat("windStrength", dimwindStrength);
  grass2d_pipeline.SetInt("texture_diffuse1", 9);

  model = glm::scale(model, glm::vec3(1, 1, 1));
  grass2d_pipeline.SetMat4("model", model);
  grass2d_pipeline.SetMat4("view", view);
  grass2d_pipeline.SetMat4("projection", projection);
  glDrawArrays(GL_TRIANGLES, 0, 6);

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

  view = glm::mat4(
      glm::mat3(camera_.view_));  // remove translation from the view matrix
  cubemaps_pipeline.SetMat4("view", view);
  cubemaps_pipeline.SetMat4("projection", projection);
  cubemaps_.Draw();

  // 1. render depth of scene to texture (from light's perspective)
  // --------------------------------------------------------------
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;
  float near_plane = 1.0f, far_plane = 7.5f;
  lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
  lightSpaceMatrix = lightProjection * lightView;
  // render scene from light's point of view
  DepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, wood_texture_.id);
  renderScene(DepthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(0, 0, 1280, 720);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render Depth map to quad for visual debugging
  // ---------------------------------------------
  DepthQuad.SetFloat("near_plane", near_plane);
  DepthQuad.SetFloat("far_plane", far_plane);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  renderQuad();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloShadowmap scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
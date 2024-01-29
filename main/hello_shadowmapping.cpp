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
#include "Shadowmap.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloShadowmapping final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void renderScene(Pipeline& shader);

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_shadowmapping/shadow_mapping.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_shadowmapping/shadow_mapping.frag";

  Pipeline pipeline;

  const std::string_view shadow_mapping_depthVertexShaderFilePath_ =
      "data/shaders/hello_shadowmapping/shadow_mapping_depth.vert";
  const std::string_view shadow_mapping_depthFragmentShaderFilePath_ =
      "data/shaders/hello_shadowmapping/shadow_mapping_depth.frag";

  Camera camera_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

  float timer_ = 0.0f;

  const std::string_view debug_quadVertexShaderFilePath_ =
      "data/shaders/hello_shadowmapping/debug_quad.vert";
  const std::string_view debug_quadFragmentShaderFilePath_ =
      "data/shaders/hello_shadowmapping/debug_quad.frag";

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

  Pipeline simpleDepthShader;
  Pipeline debugDepthQuad;

  Texture wood_texture_;
  const std::string_view wood_texture_FilePath_ = "data/textures/wood.jpg";

  // create depth texture
  // unsigned int depthMap;

  // unsigned int depthMapFBO;

  PrimitiveObjects plane_;

  Shadowmap shadowmap_;
};

void HelloShadowmapping::Begin() {
  // cubemaps_.SetUp(faces);
  // cubemaps_pipeline.CreateProgram(cubemapsVertexShaderFilePath_,
  // cubemapsFragmentShaderFilePath_);

  pipeline.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);

  simpleDepthShader.CreateProgram(shadow_mapping_depthVertexShaderFilePath_,
                                  shadow_mapping_depthFragmentShaderFilePath_);

  debugDepthQuad.CreateProgram(debug_quadVertexShaderFilePath_,
                               debug_quadFragmentShaderFilePath_);

  // cubemaps_pipeline.SetInt("skybox", 0);

  wood_texture_.is_uv_inverted = false;
  wood_texture_.TextureFromFileRepeat(wood_texture_FilePath_);
  wood_texture_.BindTexture(GL_TEXTURE0);

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  glEnable(GL_DEPTH_TEST);

  plane_.SetUpPlane();


  // configure depth map FBO
  // -----------------------
  // glGenFramebuffers(1, &depthMapFBO);

  // glGenTextures(1, &depthMap);
  // glBindTexture(GL_TEXTURE_2D, depthMap);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
  //              SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //// attach depth texture as FBO's depth buffer
  // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
  //                        depthMap, 0);
  // glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);

      shadowmap_.SetUp();
  pipeline.SetInt("diffuseTexture", 0);
  pipeline.SetInt("shadowMap", 1);

  debugDepthQuad.SetInt("depthMap", 0);
}

void HelloShadowmapping::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
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

void HelloShadowmapping::renderScene(Pipeline& shader) {
  // floor
  glm::mat4 model = glm::mat4(1.0f);
  shader.SetMat4("model", model);
  plane_.RenderPlane();
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

void HelloShadowmapping::Update(float dt) {
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  //glm::mat4 lightProjection, lightView;
  //glm::mat4 lightSpaceMatrix;
  //float near_plane = 1.0f, far_plane = 7.5f;
  //lightProjection = glm::perspective(
  //    glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT,
  //    near_plane, far_plane);  //
  //// note that if you use a perspective projection matrix you'll have to change
  //// the light position as the current light position isn't enough to reflect
  //// the whole scene
  //lightProjection =
  //    glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  //lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
  //lightSpaceMatrix = lightProjection * lightView;
  //// render scene from light's point of view
  //simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

  //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  //glBindFramebuffer(GL_FRAMEBUFFER, shadowmap_.depthMapFBO);
  //glClear(GL_DEPTH_BUFFER_BIT);
  //// glActiveTexture(GL_TEXTURE0);
  //// glBindTexture(GL_TEXTURE_2D, wood_texture_.id);
  //renderScene(simpleDepthShader);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //// reset viewport
  //glViewport(0, 0, Engine::screen_width_, Engine::screen_height_);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shadowmap_.GenerateShadowMap(lightPos);
  simpleDepthShader.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  renderScene(simpleDepthShader);
  shadowmap_.Reset();

  // 2. render scene as normal using the generated depth/shadow map
  projection = glm::perspective(glm::radians(50.0f),
                                static_cast<float>(Engine::screen_width_) /
                                    static_cast<float>(Engine::screen_height_),
                                0.1f, 100.0f);

  pipeline.SetMat4("projection", projection);
  pipeline.SetMat4("view", view);
  // set light uniforms
  pipeline.SetVec3("viewPos", camera_.position_);
  pipeline.SetVec3("lightPos", lightPos);
  pipeline.SetMat4("lightSpaceMatrix", shadowmap_.lightSpaceMatrix);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, wood_texture_.id);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadowmap_.depthMap);

  renderScene(pipeline);

  //// render Depth map to quad for visual debugging
  //// ---------------------------------------------
  // debugDepthQuad.SetFloat("near_plane", near_plane);
  // debugDepthQuad.SetFloat("far_plane", far_plane);
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, depthMap);
  //// renderQuad();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloShadowmapping scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
#include "Shadowmap.h"

void Shadowmap::SetUp() {
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

void Shadowmap::GenerateShadowMap(glm::vec3 lightPos) {
  lightProjection = glm::perspective(
      glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT,
      near_plane, far_plane);  //
  // note that if you use a perspective projection matrix you'll have to change
  // the light position as the current light position isn't enough to reflect
  // the whole scene
  lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
  lightSpaceMatrix = lightProjection * lightView;
  // render scene from light's point of view
  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void Shadowmap::Reset() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(0, 0, gpr5300::Engine::screen_width_,
             gpr5300::Engine::screen_height_);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// UPDATE 1

// glm::mat4 lightProjection, lightView;
// glm::mat4 lightSpaceMatrix;
// lightProjection = glm::perspective(
//     glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT,
//     near_plane, far_plane);  //
//// note that if you use a perspective projection matrix you'll have to change
//// the light position as the current light position isn't enough to reflect
//// the whole scene
// lightProjection =
//     glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
// lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
// lightSpaceMatrix = lightProjection * lightView;
//// render scene from light's point of view
// simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
//
// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
// glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
// glClear(GL_DEPTH_BUFFER_BIT);
// glActiveTexture(GL_TEXTURE0);
// glBindTexture(GL_TEXTURE_2D, wood_texture_.id);
// renderScene(simpleDepthShader);
// glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//// reset viewport
// glViewport(0, 0, 1280, 720);
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//// 2. render scene as normal using the generated depth/shadow map
// projection =
//     glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
//
// glm::mat4 view = camera_.view_;
// pipeline.SetMat4("projection", projection);
// pipeline.SetMat4("view", view);
//// set light uniforms
// pipeline.SetVec3("viewPos", camera_.position_);
// pipeline.SetVec3("lightPos", lightPos);
// pipeline.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
// glActiveTexture(GL_TEXTURE0);
// glBindTexture(GL_TEXTURE_2D, wood_texture_.id);
// glActiveTexture(GL_TEXTURE1);
// glBindTexture(GL_TEXTURE_2D, depthMap);
//
//// render Scene
// renderScene(pipeline);
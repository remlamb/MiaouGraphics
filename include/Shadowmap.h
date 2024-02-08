#pragma once

#include <GL/glew.h>

#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include "engine.h"
#include "pipeline.h"

class Shadowmap {
 private:
  float near_plane = 1.0f, far_plane = 7.5f;
  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

 public:
  unsigned int depthMap = 0;
  unsigned int depthMapFBO = 0;
  glm::mat4 lightProjection, lightView, lightSpaceMatrix = glm::mat4(1.0f);
  Shadowmap() = default;
  void SetUp();
  void GenerateShadowMap(glm::vec3 lightPos, glm::vec3 lightDir);
  void Reset();
};

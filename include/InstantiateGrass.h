#pragma once
#include <GL/glew.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include "PrimitiveObjects.h"

class InstantiateGrass {
 private:
  PrimitiveObjects quad_;
  unsigned int amount_ = 96;
  float elements_;

 public:
  InstantiateGrass() = default;
  void SetUp();
  void Render();
};

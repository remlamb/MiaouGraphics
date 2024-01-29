#pragma once
#include <GL/glew.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include "PrimitiveObjects.h"

class InstantiateGrass {
 private:
  PrimitiveObjects quad_;
  unsigned int amount_ = 4096;

 public:
  InstantiateGrass() = default;
  void SetUp();
  void Render();
};

#pragma once
#include <GL/glew.h>

#include <array>
#include <iostream>

#include "pipeline.h"

class FrameBuffer {
 private:
  unsigned int framebufferFBO_, framebufferRBO_, framebufferTexture_ = 0;


  std::array<float, 108> screenquadVertices_ = {
      // vertex attributes for a quad that fills the entire screen in Normalized
      // Device Coordinates.
      // positions   // texCoords
      -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
      0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

      -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
      1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  unsigned int screenquadVAO_, screenquadVBO_ = 0;

 public:
  int post_process_type_ = 0;
  FrameBuffer() = default;
  void SetUp(int width, int height);
  void SetUpColorBuffer();
  void Reset();
  void Clear();
  /**
   * @brief Draw FrameBuffer at last, give screenTexture in the pipeline before
   * Drawing.
   */
  void Bind();
  void BindRender();
  void Draw();
  void Delete();
};

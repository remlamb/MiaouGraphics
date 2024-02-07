#pragma once
#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

#include "Pipeline.h"
#include "PrimitiveObjects.h"

struct bloomMip {
  glm::vec2 size;
  glm::ivec2 intSize;
  unsigned int texture;
};

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

class BloomRenderer {
 public:
  BloomRenderer();
  ~BloomRenderer();
  bool Init(unsigned int windowWidth, unsigned int windowHeight);
  void Destroy();
  void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
  unsigned int BloomTexture();
  unsigned int BloomMip_i(int index);
  gpr5300::Pipeline mDownsampleShader;
  gpr5300::Pipeline mUpsampleShader;
  PrimitiveObjects cube_;
  PrimitiveObjects quad_;

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

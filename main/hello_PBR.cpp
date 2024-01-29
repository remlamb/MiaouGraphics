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
class HelloPBR final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_PBR/pbr.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_PBR/pbr.frag";

  const std::string_view textVertexShaderFilePath_ =
      "data/shaders/hello_PBR/pbr_text.vert";
  const std::string_view textFragmentShaderFilePath_ =
      "data/shaders/hello_PBR/pbr_text.frag";

  Pipeline pipeline;

  Texture albedo;
  const std::string_view albedoFilePath_ =
      "data/textures/rust/rust_basecolor.png";

  Texture normal;
  const std::string_view normalFilePath_ = "data/textures/rust/rust_normal.png";

  Texture metallic;
  const std::string_view metallicFilePath_ =
      "data/textures/rust/rust_metallic.png";

  Texture roughness;
  const std::string_view roughnessFilePath_ =
      "data/textures/rust/rust_roughness.png";

  Texture ao;
  const std::string_view aoFilePath_ = "data/textures/rust/rust_ao.png";

  Camera camera_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  float timer_ = 0.0f;

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

  glm::vec3 lightPositions[4] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };
  glm::vec3 lightColors[4] = {
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

  int nrRows = 7;
  int nrColumns = 7;
  float spacing = 2.5;
};

void HelloPBR::Begin() {
  // pipeline.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);
  // pipeline.SetVec3("albedo", 0.5f, 0.0f, 0.0f);
  // pipeline.SetFloat("ao", 1.0f);

  pipeline.CreateProgram(textVertexShaderFilePath_,
                         textFragmentShaderFilePath_);

  cubemaps_pipeline.CreateProgram(cubemapsVertexShaderFilePath_,
                                  cubemapsFragmentShaderFilePath_);

  cubemaps_.SetUp(faces);

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
  pipeline.SetMat4("projection", projection);

  albedo.TextureFromFile(albedoFilePath_.data());
  normal.TextureFromFile(normalFilePath_.data());
  metallic.TextureFromFile(metallicFilePath_.data());
  roughness.TextureFromFile(roughnessFilePath_.data());
  ao.TextureFromFile(aoFilePath_.data());

  albedo.BindTexture(GL_TEXTURE0);
  normal.BindTexture(GL_TEXTURE1);
  metallic.BindTexture(GL_TEXTURE2);
  roughness.BindTexture(GL_TEXTURE3);
  ao.BindTexture(GL_TEXTURE4);

  pipeline.SetInt("albedoMap", 0);
  pipeline.SetInt("normalMap", 1);
  pipeline.SetInt("metallicMap", 2);
  pipeline.SetInt("roughnessMap", 3);
  pipeline.SetInt("aoMap", 4);

  glEnable(GL_DEPTH_TEST);
}

void HelloPBR::End() {
  // Unload program/pipeline
  cubemaps_.Delete();
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere() {
  if (sphereVAO == 0) {
    glGenVertexArrays(1, &sphereVAO);

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
      if (!oddRow)  // even rows: y == 0, y == 2; and so on
      {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      } else {
        for (int x = X_SEGMENTS; x >= 0; --x) {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0) {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0) {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)(6 * sizeof(float)));
  }

  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void HelloPBR::Update(float dt) {
  model = glm::mat4(1.0f);
  timer_ += dt;

  camera_.Update(dt);
  view = camera_.view_;

  pipeline.SetMat4("view", view);
  pipeline.SetVec3("camPos", camera_.position_);

  //// render rows*column number of spheres with varying metallic/roughness
  /// values / scaled by rows and columns respectively
  // glm::mat4 model = glm::mat4(1.0f);

  // pipeline.SetVec3("albedo", 0.5f, 0.0f, 0.0f);
  // for (int row = 0; row < nrRows; ++row) {
  //   pipeline.SetFloat("metallic", (float)row / (float)nrRows);
  //   for (int col = 0; col < nrColumns; ++col) {
  //     // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces
  //     // (roughness of 0.0) tend to look a bit off on direct lighting.
  //     pipeline.SetFloat("roughness",
  //                       glm::clamp((float)col / (float)nrColumns,
  //                       0.05f, 1.0f));

  //    model = glm::mat4(1.0f);
  //    model = glm::translate(model,
  //                           glm::vec3((col - (nrColumns / 2)) * spacing,
  //                                     (row - (nrRows / 2)) * spacing, 0.0f));
  //    pipeline.SetMat4("model", model);
  //    pipeline.SetMat3("normalMatrix",
  //                     glm::transpose(glm::inverse(glm::mat3(model))));
  //    renderSphere();
  //  }
  //}

  //// render light source (simply re-render sphere at light positions)
  //// this looks a bit off as we use the same shader, but it'll make their
  //// positions obvious and keeps the codeprint small.
  // pipeline.SetVec3("albedo", 1.0f, 1.0f, 1.0f);
  // pipeline.SetFloat("metallic", 0.1f);
  // pipeline.SetFloat("roughness", 0.1f);
  // for (unsigned int i = 0;
  //      i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
  //   glm::vec3 newPos =
  //       lightPositions[i] + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
  //   newPos = lightPositions[i];
  //   pipeline.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
  //   pipeline.SetVec3("lightColors[" + std::to_string(i) + "]",
  //   lightColors[i]);

  //  model = glm::mat4(1.0f);
  //  model = glm::translate(model, newPos);
  //  model = glm::scale(model, glm::vec3(0.5f));
  //  pipeline.SetMat4("model", model);
  //  pipeline.SetMat3("normalMatrix",
  //                   glm::transpose(glm::inverse(glm::mat3(model))));
  //  renderSphere();
  //}

  // render rows*column number of spheres with material properties defined by
  // textures (they all have the same material properties)
  albedo.BindTexture(GL_TEXTURE0);
  normal.BindTexture(GL_TEXTURE1);
  metallic.BindTexture(GL_TEXTURE2);
  roughness.BindTexture(GL_TEXTURE3);
  ao.BindTexture(GL_TEXTURE4);

  glm::mat4 model = glm::mat4(1.0f);
  for (int row = 0; row < nrRows; ++row) {
    for (int col = 0; col < nrColumns; ++col) {
      model = glm::mat4(1.0f);
      model = glm::translate(
          model, glm::vec3((float)(col - (nrColumns / 2)) * spacing,
                           (float)(row - (nrRows / 2)) * spacing, 0.0f));
      pipeline.SetMat4("model", model);
      pipeline.SetMat3("normalMatrix",
                       glm::transpose(glm::inverse(glm::mat3(model))));
      renderSphere();
    }
  }

  for (unsigned int i = 0;
       i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
    glm::vec3 newPos =
        lightPositions[i] + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
    newPos = lightPositions[i];
    pipeline.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
    pipeline.SetVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

    model = glm::mat4(1.0f);
    model = glm::translate(model, newPos);
    model = glm::scale(model, glm::vec3(0.5f));
    pipeline.SetMat4("model", model);
    pipeline.SetMat3("normalMatrix",
                     glm::transpose(glm::inverse(glm::mat3(model))));
    renderSphere();
  }

  view = glm::mat4(glm::mat3(camera_.view_));
  cubemaps_pipeline.SetMat4("view", view);
  cubemaps_pipeline.SetMat4("projection", projection);
  cubemaps_.Draw();
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloPBR scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
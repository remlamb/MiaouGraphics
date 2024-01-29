#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "Model.h"
#include "engine.h"
#include "file_utility.h"
#include "pipeline.h"
#include "scene.h"

namespace gpr5300 {
class HelloNormalmap final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void renderQuad();

 private:
  const std::string_view vertexShaderFilePath_ =
      "data/shaders/hello_normalmap/normalmap.vert";
  const std::string_view fragmentShaderFilePath_ =
      "data/shaders/hello_normalmap/normalmap.frag";

  Pipeline plane_pipeline;

  const std::string_view no_vertexShaderFilePath_ =
      "data/shaders/hello_lightmaps/cube.vert";
  const std::string_view no_fragmentShaderFilePath_ =
      "data/shaders/hello_lightmaps/cube.frag";

  Pipeline no_normal_plane_pipeline;

  const std::string_view brick_diffuse_path = "data/textures/brickwall.jpg";
  const std::string_view brick_normal_path =
      "data/textures/brickwall_normal.jpg";

  Texture diffuseMap;
  Texture normalMap;

  Camera camera_;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  glm::vec3 lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
};

void HelloNormalmap::Begin() {
  plane_pipeline.CreateProgram(vertexShaderFilePath_, fragmentShaderFilePath_);
  diffuseMap.TextureFromFile(brick_diffuse_path.data());
  normalMap.TextureFromFile(brick_normal_path.data());

  no_normal_plane_pipeline.CreateProgram(no_vertexShaderFilePath_,
                                         no_fragmentShaderFilePath_);

  projection =
      glm::perspective(glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

  glEnable(GL_DEPTH_TEST);
}

void HelloNormalmap::End() {}

void HelloNormalmap::Update(float dt) {
  camera_.Update(dt);
  view = camera_.view_;

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
  model = glm::rotate(model, -0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
  plane_pipeline.SetInt("diffuseMap", 0);
  plane_pipeline.SetInt("normalMap", 1);

  plane_pipeline.SetMat4("projection", projection);
  plane_pipeline.SetMat4("view", view);
  plane_pipeline.SetMat4("model", model);
  plane_pipeline.SetVec3("viewPos", camera_.position_);
  plane_pipeline.SetVec3("lightPos", lightPos);
  diffuseMap.BindTexture(GL_TEXTURE0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, diffuseMap.id);
  normalMap.BindTexture(GL_TEXTURE1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, normalMap.id);
  renderQuad();
}
void HelloNormalmap::renderQuad() {
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;
  if (quadVAO == 0) {
    // positions
    glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 pos4(1.0f, 1.0f, 0.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    float quadVertices[] = {
        // positions            // normal         // texcoords  // tangent //
        // bitangent
        pos1.x,       pos1.y,       pos1.z,       nm.x,         nm.y,
        nm.z,         uv1.x,        uv1.y,        tangent1.x,   tangent1.y,
        tangent1.z,   bitangent1.x, bitangent1.y, bitangent1.z, pos2.x,
        pos2.y,       pos2.z,       nm.x,         nm.y,         nm.z,
        uv2.x,        uv2.y,        tangent1.x,   tangent1.y,   tangent1.z,
        bitangent1.x, bitangent1.y, bitangent1.z, pos3.x,       pos3.y,
        pos3.z,       nm.x,         nm.y,         nm.z,         uv3.x,
        uv3.y,        tangent1.x,   tangent1.y,   tangent1.z,   bitangent1.x,
        bitangent1.y, bitangent1.z,

        pos1.x,       pos1.y,       pos1.z,       nm.x,         nm.y,
        nm.z,         uv1.x,        uv1.y,        tangent2.x,   tangent2.y,
        tangent2.z,   bitangent2.x, bitangent2.y, bitangent2.z, pos3.x,
        pos3.y,       pos3.z,       nm.x,         nm.y,         nm.z,
        uv3.x,        uv3.y,        tangent2.x,   tangent2.y,   tangent2.z,
        bitangent2.x, bitangent2.y, bitangent2.z, pos4.x,       pos4.y,
        pos4.z,       nm.x,         nm.y,         nm.z,         uv4.x,
        uv4.y,        tangent2.x,   tangent2.y,   tangent2.z,   bitangent2.x,
        bitangent2.y, bitangent2.z};
    // configure plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void*)(11 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloNormalmap scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
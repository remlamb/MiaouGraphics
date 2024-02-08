#include "InstantiateGrass.h"

void InstantiateGrass::SetUp() {
  // Quad Grass2DMesh
  quad_.SetUpQuad();

  // Position Pour L'herbe
  glm::mat4* modelMatrices;
  modelMatrices = new glm::mat4[3 * amount_];
  float separation = 0.12f;  // Adjust the separation between objects
  float amout_squareroot = sqrt(amount_);
  float pos = 10;
  int i = 0;
  int rotationPreset = 0;
  for (unsigned int zIndex = 0; zIndex < amout_squareroot; zIndex++) {
    for (unsigned int xIndex = 0; xIndex < amout_squareroot; xIndex++) {
      glm::mat4 model = glm::mat4(1.0f);
      float z = static_cast<float>(zIndex) * separation;
      float x = static_cast<float>(xIndex) * separation;
      float y = 0.5f;
      model = glm::translate(model, glm::vec3(x, y, z));

      // TODO better random Scaling
      float scale = static_cast<float>((rand() % 20) / 50.0 + 0.05);
      model = glm::scale(model, glm::vec3(scale));

      // Rotation
      modelMatrices[i] = model;
      i++;

      model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrices[i] = model;
      i++;

      model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrices[i] = model;
      i++;
    }
  }

  // configure instanced array
  // -------------------------
  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, amount_ * sizeof(glm::mat4), &modelMatrices[0],
               GL_STATIC_DRAW);

  glBindVertexArray(quad_.GetQuadVAO());
  // set attribute pointers for matrix (4 times vec4)
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(3 * sizeof(glm::vec4)));

  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);
}

void InstantiateGrass::Render() {
  glBindVertexArray(quad_.GetQuadVAO());
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6,
                        amount_);  // 100 triangles of 6 vertices each
  glBindVertexArray(0);
}
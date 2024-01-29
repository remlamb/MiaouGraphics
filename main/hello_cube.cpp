#include <GL/glew.h>

#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"
#include "file_utility.h"
#include "scene.h"
#include "stb_image.h"

namespace gpr5300 {
class HelloCube final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void LoadTexture(GLuint& texture, const char* file_path);
  void CreateProgram(std::string_view vertex_shader_file_path,
                     std::string_view fragment_shader_file_path);

 private:
  GLuint vertexShader_ = 0;
  GLuint fragmentShader_ = 0;
  GLuint program_ = 0;
  GLuint vao_ = 0;
  GLuint texture1_ = 0;
  GLuint texture2_ = 0;
  float timer_ = 0;
  GLuint vbo_[2] = {};
  GLuint ebo_ = 0;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  // float vertices[108] = {
  //     -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
  //     0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

  //    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
  //    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

  //    -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
  //    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

  //    0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
  //    0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

  //    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
  //    0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

  //    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
  //    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,

  //};

  float vertices[72] = {


      0.5f,  0.5f,  0.5f,

      0.5f,  -0.5f, 0.5f,

      -0.5f, -0.5f, 0.5f,

      -0.5f, 0.5f,  0.5f,

      0.5f,  0.5f,  -0.5f,

      0.5f,  0.5f,  0.5f,

      -0.5f, 0.5f,  0.5f,

      -0.5f, 0.5f,  -0.5f,

      0.5f,  0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f,

      -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f,

      0.5f,  -0.5f, 0.5f,

      -0.5f, -0.5f, 0.5f,

      -0.5f, -0.5f, -0.5f,

      0.5f,  0.5f,  -0.5f,

      0.5f,  -0.5f, -0.5f,

      0.5f,  -0.5f, 0.5f,

      0.5f,  0.5f,  0.5f,

      -0.5f, 0.5f,  0.5f,

      -0.5f, -0.5f, 0.5f,

      -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f,


  };

  unsigned int indices[36] = {
      0,  1,  3,  1,  2,  3,  4,  5,  7,  5,  6,  7,  8,  9,  11, 9,  10, 11,
      12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 20, 21, 23, 21, 22, 23,
  };

  float texCoords[48] = {

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,

      1.0f, 1.0f,

      1.0f, 0.0f,

      0.0f, 0.0f,

      0.0f, 1.0f,
  };

  glm::vec3 cubePositions[10] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
};

void HelloCube::LoadTexture(GLuint& texture, const char* file_path) {
  stbi_set_flip_vertically_on_load(true);
  // GLuint texture = 0;
  int width, height, nbrChannels;
  unsigned char* data = stbi_load(file_path, &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n";
    // return texture;
  }

  std::cout << "width : " << width << " height : " << height
            << " Channels : " << nbrChannels << '\n';

  // Load Texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  // return texture;
}

void HelloCube::CreateProgram(std::string_view vertex_shader_file_path,
                              std::string_view fragment_shader_file_path) {
  // Load shaders
  const auto vertexContent = LoadFile(vertex_shader_file_path);
  const auto* ptr = vertexContent.data();
  vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader_, 1, &ptr, nullptr);
  glCompileShader(vertexShader_);

  // Check success status of shader compilation
  GLint success;
  glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cerr << "Error while loading vertex shader\n";
  }
  const auto fragmentContent = LoadFile(fragment_shader_file_path);
  ptr = fragmentContent.data();
  fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader_, 1, &ptr, nullptr);
  glCompileShader(fragmentShader_);

  // Check success status of shader compilation
  glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cerr << "Error while loading fragment shader\n";
  }
  // Load program/pipeline
  program_ = glCreateProgram();
  glAttachShader(program_, vertexShader_);
  glAttachShader(program_, fragmentShader_);
  glLinkProgram(program_);

  // Check if shader program was linked correctly
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (!success) {
    std::cerr << "Error while linking shader program\n";
  }
}

void HelloCube::Begin() {
  LoadTexture(texture1_, "data/textures/deep.png");
  LoadTexture(texture2_, "data/textures/deep2.png");
  CreateProgram("data/shaders/hello_cube/texture.vert",
                "data/shaders/hello_cube/texture.frag");

  // TODO Struct Mesh avec vao vbo ebo (tab de vbo ou vbo avec nom unique)
  // TODO Fonction ou on link les vbo au vao -> avec un ordre defini
  glCreateVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &vbo_[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glEnable(GL_DEPTH_TEST);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  projection =
      glm::perspective(glm::radians(50.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

void HelloCube::End() {
  // Unload program/pipeline
  glDeleteProgram(program_);
  glDeleteShader(vertexShader_);
  glDeleteShader(fragmentShader_);
  glDeleteBuffers(2, &vbo_[0]);
  glDeleteBuffers(1, &ebo_);
  // TODO Mettre warning dans le destructeur si on oublie de Delete Texture
  // Peut etre fait pour les autres elements aussi
  glDeleteTextures(1, &texture1_);
  glDeleteTextures(1, &texture2_);
  glDeleteVertexArrays(1, &vao_);
}

void HelloCube::Update(float dt) {
  // Draw program
  glUseProgram(program_);
  glBindVertexArray(vao_);

  timer_ += dt;
  const float value = (std::cos(timer_ * 1.5f) + 1.0f) / 2.0f;
  glUniform1f(glGetUniformLocation(program_, "value"), value);

  // TODO function Active Texture -> on peut utiliser un int pour counter de
  // TODO  GL_texture et gl_texture + ce int pour activer le bon.
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, texture1_);
  //glUniform1i(glGetUniformLocation(program_, "tex1"), 0);

  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_2D, texture2_);
  //glUniform1i(glGetUniformLocation(program_, "tex2"), 1);

  // model =
  //     glm::rotate(model, glm::radians(dt * 28), glm::vec3(1.0f, 0.0f, 1.0f));

  //glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE,
  //                   glm::value_ptr(model));
  //glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE,
  //                   glm::value_ptr(view));
  //glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE,
  //                   glm::value_ptr(projection));

  for (unsigned int i = 0; i < 10; i++) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1_);
    glUniform1i(glGetUniformLocation(program_, "tex1"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2_);
    glUniform1i(glGetUniformLocation(program_, "tex2"), 1);

    float angle =  timer_ * 80 + 20.0f * i;
    model =
        glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));

    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
  }
  // glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloCube scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
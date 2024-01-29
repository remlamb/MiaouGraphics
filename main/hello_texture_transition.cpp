#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"
#include "file_utility.h"
#include "scene.h"
#include "stb_image.h"

namespace gpr5300 {
class HelloTextureTransition final : public Scene {
 public:
  void Begin() override;
  void End() override;
  void Update(float dt) override;
  void LoadTexture(GLuint& texture, const char* file_path);
  void CreateProgram(const char* file_path_vertex_shader,
                     const char* file_path_fragment_shader);

 private:
  GLuint vertexShader_ = 0;
  GLuint fragmentShader_ = 0;
  GLuint program_ = 0;
  GLuint vao_ = 0;
  GLuint texture1_ = 0;
  GLuint texture2_ = 0;

  float timer_ = 0;

  float vertices[12] = {-0.5f, 0.5f,  0.0f, 0.5f,  0.5f,  0.0f,
                        0.5f,  -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};

  unsigned int indices[6] = {
      0, 3, 2,  // first triangle
      0, 2, 1   // second triangle
  };

  float texCoords[8] = {
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  };
};

void HelloTextureTransition::LoadTexture(GLuint& texture,
                                         const char* file_path) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nbrChannels;
  unsigned char* data = stbi_load(file_path, &width, &height, &nbrChannels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image\n";
  } else {
    std::cout << "width : " << width << "height : " << height
              << "Channels : " << nbrChannels;
  }
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
}

void HelloTextureTransition::CreateProgram(
    const char* file_path_vertex_shader,
    const char* file_path_fragment_shader) {
  // Load shaders
  const auto vertexContent = LoadFile(file_path_vertex_shader);
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
  const auto fragmentContent = LoadFile(file_path_fragment_shader);
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

void HelloTextureTransition::Begin() {
  LoadTexture(texture1_, "data/textures/deep.png");
  LoadTexture(texture2_, "data/textures/deep2.png");
  CreateProgram("data/shaders/hello_texture_transition/texture.vert",
                "data/shaders/hello_texture_transition/texture.frag");

  // Empty vao
  GLuint vbo_[2] = {};
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

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
}

void HelloTextureTransition::End() {
  // Unload program/pipeline
  glDeleteProgram(program_);
  glDeleteShader(vertexShader_);
  glDeleteShader(fragmentShader_);
  glDeleteVertexArrays(1, &vao_);
}

void HelloTextureTransition::Update(float dt) {
  // Draw program
  glUseProgram(program_);
  glBindVertexArray(vao_);

  timer_ += dt * 1.5f;
  const float value = (std::cos(timer_) + 1.0f) / 2.0f;
  glUniform1f(glGetUniformLocation(program_, "value"), value);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1_);
  glUniform1i(glGetUniformLocation(program_, "tex1"), 0);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2_);
  glUniform1i(glGetUniformLocation(program_, "tex2"), 1);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
}  // namespace gpr5300
int main(int argc, char** argv) {
  gpr5300::HelloTextureTransition scene;
  gpr5300::Engine engine(&scene);
  engine.Run();

  return EXIT_SUCCESS;
}
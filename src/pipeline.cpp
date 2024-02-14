#include "pipeline.h"

namespace gpr5300 {

GLuint Pipeline::CreateProgram(std::string_view vertex_shader_file_path,
                               std::string_view fragment_shader_file_path) {
  // Load shaders
  const auto vertexContent = LoadFile(vertex_shader_file_path);
  const auto* ptr = vertexContent.data();
  auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &ptr, nullptr);
  glCompileShader(vertexShader);

  // Check success status of shader compilation
  GLint success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cerr << "Error while loading vertex shader\n";
  }
  const auto fragmentContent = LoadFile(fragment_shader_file_path);
  ptr = fragmentContent.data();
  auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &ptr, nullptr);
  glCompileShader(fragmentShader);

  // Check success status of shader compilation
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cerr << "Error while loading fragment shader\n";
  }
  // Load program/pipeline
  program_ = glCreateProgram();
  glAttachShader(program_, vertexShader);
  glAttachShader(program_, fragmentShader);
  glLinkProgram(program_);

  // Check if shader program was linked correctly
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (!success) {
    std::cerr << "Error while linking shader program\n";
    return 0;
  } else {
    return program_;
  }
}

void Pipeline::SetInt(const std::string_view uniform, GLint glint) {
  Use();
  glUniform1i(glGetUniformLocation(program_, uniform.data()), glint);
}

void Pipeline::SetVec3(const std::string_view uniform, float vec3x, float vec3y,
                       float vec3z) {
  Use();
  glm::vec3 vec3(vec3x, vec3y, vec3z);
  glUniform3fv(glGetUniformLocation(program_, uniform.data()), 1,
               glm::value_ptr(vec3));
}

void Pipeline::SetVec3(const std::string_view uniform, glm::vec3 vec3) {
  Use();
  glUniform3fv(glGetUniformLocation(program_, uniform.data()), 1,
               glm::value_ptr(vec3));
}

	void Pipeline::SetVec4(const std::string_view uniform, glm::vec4 vec4) {
  Use();
  glUniform4fv(glGetUniformLocation(program_, uniform.data()), 1,
               glm::value_ptr(vec4));
}

void Pipeline::SetVec2(const std::string_view uniform, glm::vec2 vec2) {
  Use();
  glUniform2fv(glGetUniformLocation(program_, uniform.data()), 1,
               glm::value_ptr(vec2));
}

void Pipeline::SetMat4(const std::string_view uniform, glm::mat4 mat4) {
  Use();
  glUniformMatrix4fv(glGetUniformLocation(program_, uniform.data()), 1,
                     GL_FALSE, glm::value_ptr(mat4));
}

void Pipeline::SetMat3(const std::string_view uniform, glm::mat3 mat3) {
  Use();
  glUniformMatrix3fv(glGetUniformLocation(program_, uniform.data()), 1,
                     GL_FALSE, glm::value_ptr(mat3));
}

void Pipeline::SetFloat(const std::string_view uniform, float value) {
  Use();
  glUniform1f(glGetUniformLocation(program_, uniform.data()), value);
}

void Pipeline::SetBool(const std::string_view uniform, bool value) {
  SetInt(uniform, value);
}

void Pipeline::Use() {
  if (current_program_ != program_) {
    glUseProgram(program_);
    current_program_ = program_;
  }
}

Pipeline::~Pipeline() {
  if (program_ != 0) {
    glDeleteProgram(program_);
  }
}
}  // namespace gpr5300
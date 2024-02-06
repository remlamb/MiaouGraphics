#pragma once
#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "file_utility.h"

namespace gpr5300 {
class Pipeline {
 public:
  GLuint program_ = 0;
  inline static GLuint current_program_ = 0;

  GLuint CreateProgram(std::string_view vertex_shader_file_path,
                       std::string_view fragment_shader_file_path);
  void SetInt(const std::string_view uniform, GLint glint);
  void SetVec3(const std::string_view uniform, float vec3x, float vec3y,
               float vec3z);

  void SetVec3(const std::string_view uniform, glm::vec3 vec3);

  void SetVec2(const std::string_view uniform, glm::vec2 vec2);

  void SetMat4(const std::string_view uniform, glm::mat4 mat4);

  void SetMat3(const std::string_view uniform, glm::mat3 mat3);

  void SetFloat(const std::string_view uniform, float value);

  void Use();

  ~Pipeline();
};
}  // namespace gpr5300

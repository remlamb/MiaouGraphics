#pragma once
#include <SDL_events.h>

#include <glm/ext/matrix_transform.hpp>

class Camera {


  glm::vec3 target_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 direction_ = glm::normalize(position_ - target_);


  glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 right_ = glm::normalize(glm::cross(up_, direction_));

  const float move_speed_ = 1.5f;

  float yaw_ = -90.0f;
  float pitch_ = 0.0f;
  const float sensitivity_ = 0.1f;

  const glm::vec3 world_up_ = up_;

 public:
  Camera() = default;
  void Update(const float dt);
  void Move(const float dt) noexcept;
  void Rotate() noexcept;
  glm::mat4 view_;
  glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
};

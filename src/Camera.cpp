#include "Camera.h"

void Camera::Update(const float dt) {
  Rotate();
  Move(dt);
  view_ = glm::lookAt(position_, position_ + front_, up_);
}

void Camera::Move(const float dt) noexcept {
  // Check keys that are currently pressed
  const Uint8* keys = SDL_GetKeyboardState(nullptr);

  if (keys[SDL_SCANCODE_W]) {
    position_ += front_ * (move_speed_ * dt);
  }
  if (keys[SDL_SCANCODE_S]) {
    position_ -= front_ * (move_speed_ * dt);
  }
  if (keys[SDL_SCANCODE_A]) {
    position_ -= right_ * (move_speed_ * dt);
  }
  if (keys[SDL_SCANCODE_D]) {
    position_ += right_ * (move_speed_ * dt);
  }
  if (keys[SDL_SCANCODE_SPACE]) {
    position_ += up_ * (move_speed_ * dt);
  }
  if (keys[SDL_SCANCODE_LCTRL]) {
    position_ -= up_ * (move_speed_ * dt);
  }
}

void Camera::Rotate() noexcept {
  int pos_x, pos_y;
  SDL_GetRelativeMouseState(&pos_x, &pos_y);

  yaw_ += pos_x * sensitivity_;
  pitch_ -= pos_y * sensitivity_;

  if (pitch_ > 89.0f) pitch_ = 89.0f;
  if (pitch_ < -89.0f) pitch_ = -89.0f;

  // calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front.y = sin(glm::radians(pitch_));
  front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front_ = glm::normalize(front);
  // also re-calculate the Right and Up vector
  right_ = glm::normalize(glm::cross(
      front_, world_up_));  // normalize the vectors, because their length gets
                            // closer to 0 the more you look up or down which
                            // results in slower movement.
  up_ = glm::normalize(glm::cross(right_, front_));
}
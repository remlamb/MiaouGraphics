#pragma once
#include "scene.h"

namespace gpr5300 {

class Engine {
 public:
  inline static int screen_width_ = 1280;
  inline static int screen_height_ = 720;
  Engine(Scene* scene);
  void Run();

 private:
  void Begin();
  void End();
  Scene* scene_ = nullptr;
  SDL_Window* window_ = nullptr;
  SDL_GLContext glRenderContext_{};
};

}  // namespace gpr5300

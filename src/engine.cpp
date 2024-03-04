#include "engine.h"

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <cassert>
#include <chrono>
#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif
#include <glm/vec2.hpp>

namespace gpr5300 {
Engine::Engine(Scene* scene) : scene_(scene) {}

void Engine::Run() {
  Begin();
  bool isOpen = true;

  std::chrono::time_point<std::chrono::system_clock> clock =
      std::chrono::system_clock::now();
  while (isOpen) {
    const auto start = std::chrono::system_clock::now();
    using seconds = std::chrono::duration<float, std::ratio<1, 1>>;
    const auto dt = std::chrono::duration_cast<seconds>(start - clock);
    clock = start;

    // Manage SDL event
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          isOpen = false;
          break;
        case SDL_WINDOWEVENT: {
          switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
              isOpen = false;
              break;
            case SDL_WINDOWEVENT_RESIZED: {
              const auto new_size =
                  glm::uvec2(event.window.data1, event.window.data2);

              Engine::screen_width_ = new_size.x;
              Engine::screen_height_ = new_size.y;

              std::cout << Engine::screen_width_ << " , "
                        << Engine::screen_height_ << std::endl;

              glViewport(0, 0, new_size.x, new_size.y);

              break;
            }
            default:
              break;
          }
          break;
        }
        default:
          break;
      }
      scene_->OnEvent(event);
      ImGui_ImplSDL2_ProcessEvent(&event);
    }
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene_->Update(dt.count());

    // Generate new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window_);
    ImGui::NewFrame();

    scene_->DrawImGui();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window_);
#ifdef TRACY_ENABLE
    FrameMark;
#endif
  }
  End();
}

void Engine::Begin() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
  // Set our OpenGL version.
#if true
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  auto windowSize = glm::ivec2(screen_width_, screen_height_);
  window_ = SDL_CreateWindow(
      "Miaou Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      windowSize.x, windowSize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  glRenderContext_ = SDL_GL_CreateContext(window_);
  // setting vsync
  SDL_GL_SetSwapInterval(1);

  if (GLEW_OK != glewInit()) {
    assert(false && "Failed to initialize OpenGL context");
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Keyboard Gamepad
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();
  ImGui_ImplSDL2_InitForOpenGL(window_, glRenderContext_);
  ImGui_ImplOpenGL3_Init("#version 300 es");

  scene_->Begin();
}

void Engine::End() {
  scene_->End();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(glRenderContext_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}
}  // namespace gpr5300

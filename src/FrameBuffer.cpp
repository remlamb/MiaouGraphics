#include "FrameBuffer.h"

#include <SDL_keyboard.h>
#include <SDL_scancode.h>

void FrameBuffer::SetUp(int width, int height) {
  glGenFramebuffers(1, &framebufferFBO_);
  glGenRenderbuffers(1, &framebufferRBO_);

  glBindFramebuffer(GL_FRAMEBUFFER, framebufferFBO_);
  glBindRenderbuffer(GL_RENDERBUFFER, framebufferRBO_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, framebufferRBO_);
}

void FrameBuffer::SetUpColorBuffer() {
  // Set up fbuffer
  SetUp(1280, 720);

  glGenTextures(1, &framebufferTexture_);
  glBindTexture(GL_TEXTURE_2D, framebufferTexture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         framebufferTexture_, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! '\n'";
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Set up screenQuad
  glGenVertexArrays(1, &screenquadVAO_);
  glGenBuffers(1, &screenquadVBO_);
  glBindVertexArray(screenquadVAO_);
  glBindBuffer(GL_ARRAY_BUFFER, screenquadVBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenquadVertices_),
               &screenquadVertices_, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void*)(2 * sizeof(float)));
}

void FrameBuffer::Reset() {
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferFBO_);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::Clear() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // clear all relevant buffers
  glClearColor(1.0f, 1.0f, 1.0f,
               1.0f);  // set clear color to white (not really necessary
                       // actually,  since /
                       // we won't be able to see behind the quad anyways)
  glClear(GL_COLOR_BUFFER_BIT);
}
void FrameBuffer::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, framebufferFBO_); }
void FrameBuffer::BindRender() {
  glBindRenderbuffer(GL_RENDERBUFFER, framebufferRBO_);
}
void FrameBuffer::Draw() {
  // input given to the shader
  const Uint8* keys = SDL_GetKeyboardState(nullptr);
  if (keys[SDL_SCANCODE_0]) {
    post_process_type_ = 0;
  }
  if (keys[SDL_SCANCODE_8]) {
    post_process_type_ = 1;
  }
  if (keys[SDL_SCANCODE_9]) {
    post_process_type_ = 2;
  }

  glBindVertexArray(screenquadVAO_);
  glBindTexture(GL_TEXTURE_2D,
                framebufferTexture_);  // use the color attachment texture as //
                                       // texture of the quad plane  the /
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::Delete() {
  glDeleteFramebuffers(1, &framebufferFBO_);
  glDeleteFramebuffers(1, &framebufferRBO_);
  glDeleteFramebuffers(1, &framebufferTexture_);

  glDeleteFramebuffers(1, &screenquadVAO_);
  glDeleteFramebuffers(1, &screenquadVBO_);
}

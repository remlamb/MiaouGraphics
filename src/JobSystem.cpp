#include "JobSystem.h"

#ifdef TRACY_ENABLE
#include <TracyC.h>

#include <Tracy.hpp>
#endif  // TRACY_ENABLE

// Job::Job(Job&& other) noexcept {
//   status_ = std::move(other.status_);
//   other.status_ = JobStatus::None;
// }
//
// Job& Job::operator=(Job&& other) noexcept {
//   status_ = std::move(other.status_);
//   other.status_ = JobStatus::None;
//
//   return *this;
// }

void Job::Execute() {
  for (const auto& dependency : dependencies_) {
    if (!dependency->IsDone()) {
      dependency->WaitUntilJobIsDone();
    }
  }
  Work();

  promise_.set_value();
  status_ = JobStatus::Done;
}

void Job::AddDependency(const Job* job) noexcept {
  dependencies_.push_back(job);
}

void Job::WaitUntilJobIsDone() const noexcept { shared_future_.get(); }

void Worker::Start() { thread_ = std::thread(&Worker::WorkLoop, this); }

void Worker::WorkLoop() {
  while (is_running_) {
    Job* job = nullptr;

    if (!jobs_->empty()) {
      job = jobs_->front();
      jobs_->pop();
    } else {
      is_running_ = false;
      break;
    }

    if (job) {
      job->Execute();
    }
  }
}

void Worker::Join() { thread_.join(); }

void JobSystem::JoinWorkers() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  for (auto& worker : workers_) {
    worker.Join();
  }
}

void JobSystem::LaunchWorkers(const int worker_count) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE

  workers_.reserve(worker_count);

  for (int i = 0; i < worker_count; i++) {
    switch (static_cast<JobType>(i)) {
      case JobType::ImageFileLoading:
        workers_.emplace_back(&read_texture_jobs_);
        workers_[i].Start();
        break;
      case JobType::ImageFileDecompressing:
        workers_.emplace_back(&decompressed_texture_jobs);
        workers_[i].Start();
        break;
      case JobType::TextureToGPU:
      case JobType::None:
        break;
    }
  }
  static bool is_running = true;
  while (is_running) {
    Job* job = nullptr;

    if (!text_to_gpu.empty()) {
      job = text_to_gpu.front();
      text_to_gpu.pop();
    } else {
      is_running = false;
      break;
    }

    if (job) {
      job->Execute();
    }
  }
}

void JobSystem::AddJob(Job* job) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE

  switch (job->type()) {
    case JobType::ImageFileLoading:
      read_texture_jobs_.push(job);
      break;
    case JobType::ImageFileDecompressing:
      decompressed_texture_jobs.push(job);
      break;
    case JobType::TextureToGPU:
      text_to_gpu.push(job);
      break;
  }
}

void ReadTextureJob::ReadTextureAsync(const std::string_view file_path) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  std::ifstream file(file_path.data(), std::ios::binary);

  if (!file.is_open()) {
    file_buffer_->data_ = nullptr;
    file_buffer_->filesize_ = 0;
  }

  file.seekg(0, std::ios::end);
  file_buffer_->filesize_ = static_cast<int>(file.tellg());
  file.seekg(0, std::ios::beg);
  file_buffer_->data_ = new unsigned char[file_buffer_->filesize_];

  file.read(reinterpret_cast<char*>(file_buffer_->data_),
            file_buffer_->filesize_);
}

void ReadTextureJob::Work() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  ReadTextureAsync(file_path_);
}

void DecompressTextureJob::DecompressTexture(unsigned char* data) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif
  stbi_set_flip_vertically_on_load(is_uv_inverted);
  texture_buffer_->imgData_ = stbi_load_from_memory(
      data, file_buffer_->filesize_, &texture_buffer_->width_,
      &texture_buffer_->height_, &texture_buffer_->nbrChannels_, 0);
}

void DecompressTextureJob::Work() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  DecompressTexture(file_buffer_->data_);
}

void TextureToGPUJob::TextureToGPU() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif
  // Load Texture
  glGenTextures(1, &texture_buffer_->id);
  glBindTexture(GL_TEXTURE_2D, texture_buffer_->id);
  if (!isRepeated) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  GLint internalFormat = 0;
  GLenum format = 0;
  if (texture_buffer_->nbrChannels_ == 1) {
    internalFormat = GL_RED;
    format = GL_RED;
  } else if (texture_buffer_->nbrChannels_ == 2) {
    internalFormat = GL_RG;
    format = GL_RG;
  } else if (texture_buffer_->nbrChannels_ == 3) {
    internalFormat = srgb_ ? GL_SRGB : GL_RGB;
    format = GL_RGB;
  } else if (texture_buffer_->nbrChannels_ == 4) {
    internalFormat = srgb_ ? GL_SRGB_ALPHA : GL_RGBA;
    format = GL_RGBA;
  } else {
    std::cerr << "Texture channel nbr are not suitable"
              << texture_buffer_->nbrChannels_ << "\n";
  }

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture_buffer_->width_,
               texture_buffer_->height_, 0, format, GL_UNSIGNED_BYTE,
               texture_buffer_->imgData_);

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(texture_buffer_->imgData_);
}

void TextureToGPUJob::Work() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  TextureToGPU();
}

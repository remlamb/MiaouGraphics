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
  Work();
  status_ = JobStatus::Done;
}

// Add fonction loop au lieu de passer par la lambda
//donné la queue au worker pour avoid le dependence, outparam
void Worker::Start(std::vector<Job*>& jobs) {
  thread_ = std::thread(&Worker::WorkLoop, this,std::reference_wrapper<std::vector<Job*>>(jobs));
}

void Worker::WorkLoop(std::vector<Job*>& jobs_queue) {
  while (is_running_) {
    Job* job = nullptr;

    if (!jobs_queue.empty()) {
      job = jobs_queue.front();
      jobs_queue.erase(jobs_queue.begin());
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
    workers_.emplace_back();
    workers_[i].Start(read_texture_jobs_);
  }
}

void JobSystem::AddJob(Job* job) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  read_texture_jobs_.push_back(job);
}

void ReadTextureJob::ReadTextureAsync(const std::string_view file_path) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  std::ifstream file(file_path.data(), std::ios::binary);

  if (!file.is_open()) {
    data_ = nullptr;
    filesize_ = 0;
  }

  file.seekg(0, std::ios::end);
  filesize_ = static_cast<int>(file.tellg());
  file.seekg(0, std::ios::beg);
  data_ = new unsigned char[filesize_];
  std::cout << "file read" << '\n';

  file.read(reinterpret_cast<char*>(data_), filesize_);
}

void ReadTextureJob::Work() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif  // TRACY_ENABLE
  ReadTextureAsync(file_path_);
}

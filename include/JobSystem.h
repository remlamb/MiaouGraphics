#pragma once

#include <future>
#include <thread>
#include <vector>

enum class JobStatus : std::int16_t {
  Started,
  Done,
  None,
};

class Job {
 public:
  Job() = default;
  virtual ~Job() = default;

  Job(Job&& other) noexcept = default;
  Job& operator=(Job&& other) noexcept = default;
  Job(const Job& other) noexcept = delete;
  Job& operator=(const Job& other) noexcept = delete;

  void Execute();

  bool IsDone() const { return status_ == JobStatus::Done; }
  bool HasStarted() const { return status_ == JobStatus::Started; }
  virtual void Work(){}

 protected:
  JobStatus status_ = JobStatus::None;
};

class Worker {
 public:
  Worker() = default;
  void Start(std::vector<Job*>& jobs);
  void Join();

 private:
  void WorkLoop(std::vector<Job*>& jobs_queue);
  std::thread thread_{};
  bool is_running_ = true;
};

class JobSystem {
 public:
  JobSystem() = default;
  void AddJob(Job* job);
  void LaunchWorkers(int worker_count);

  void JoinWorkers();

  std::vector<Job*> read_texture_jobs_{}; // switch to queue

 private:
  std::vector<Worker> workers_{};
};

class ReadTextureJob final : public Job {
 public:
  ReadTextureJob(std::string_view file_path) : file_path_(file_path) {}
  void ReadTextureAsync(std::string_view file_path);

  // ReadTextureJob(ReadTextureJob&& other) noexcept = default;
  // ReadTextureJob& operator=(ReadTextureJob&& other) noexcept = default;
  // ReadTextureJob(const ReadTextureJob& other) noexcept = delete;
  // ReadTextureJob& operator=(const ReadTextureJob& other) noexcept =
  // delete;

  ~ReadTextureJob() override {
    data_ = nullptr;
    file_path_ = nullptr;
  }

  void Work() override;

 private:
  int filesize_ = 0;
  unsigned char* data_ = nullptr;
  std::string_view file_path_ = nullptr;
};
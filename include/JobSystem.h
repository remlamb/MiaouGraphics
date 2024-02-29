#pragma once

#include <future>
#include <queue>
#include <thread>
#include <vector>

enum class JobStatus : std::int16_t {
  Started,
  Done,
  None,
};

enum class JobType : std::int16_t {
  None = -1,
  ImageFileLoading,
  ImageFileDecompressing,
  TextureToGPU,
};

struct FileBuffer {
  int filesize_ = 0;
  unsigned char* data_ = nullptr;
  ~FileBuffer() {
    delete[] data_;
    filesize_ = 0;
  };
};

struct TextureBuffer {
  int width_ = 0;
  int height_ = 0;
  int nbrChannels_ = 0;
  unsigned char* imgData_ = nullptr;
  GLuint id = 0;
  ~TextureBuffer() = default;
};

class Job {
 public:
  Job() = default;
  Job(const JobType job_type) : type_(job_type) {}
  virtual ~Job() = default;

  Job(Job&& other) noexcept = default;
  Job& operator=(Job&& other) noexcept = default;
  Job(const Job& other) noexcept = delete;
  Job& operator=(const Job& other) noexcept = delete;

  void Execute();
  void AddDependency(const Job* job) noexcept;
  bool IsReadyToStart() noexcept;

  void WaitUntilJobIsDone() const noexcept;

  bool IsDone() const { return status_ == JobStatus::Done; }
  bool HasStarted() const { return status_ == JobStatus::Started; }
  virtual void Work() = 0;

  JobType type() const noexcept { return type_; }

 protected:
  std::vector<const Job*> dependencies_;
  std::promise<void> promise_;
  std::shared_future<void> shared_future_ = promise_.get_future();
  JobStatus status_ = JobStatus::None;
  JobType type_ = JobType::None;
};

class Worker {
 public:
  explicit Worker(std::queue<Job*>* jobs) : jobs_(jobs) {}
  void Start();
  void Join();

 private:
  void WorkLoop();
  std::queue<Job*>* jobs_;
  std::thread thread_{};
  bool is_running_ = true;
};

class JobSystem {
 public:
  JobSystem() = default;
  void AddJob(Job* job);
  void LaunchWorkers(int worker_count);

  void JoinWorkers();

  std::queue<Job*> read_texture_jobs_{};  // switch to queue
  std::queue<Job*> decompressed_texture_jobs{};

 private:
  std::vector<Worker> workers_{};
};

class ReadTextureJob final : public Job {
 public:
  ReadTextureJob() = default;
  ReadTextureJob(std::string_view file_path, FileBuffer* file_buffer)
      : Job(JobType::ImageFileLoading),
        file_path_(file_path.data()),
        file_buffer_(file_buffer) {}
  void ReadTextureAsync(std::string_view file_path);

  ReadTextureJob(ReadTextureJob&& other) noexcept = default;
  ReadTextureJob& operator=(ReadTextureJob&& other) noexcept = default;
  ReadTextureJob(const ReadTextureJob& other) noexcept = delete;
  ReadTextureJob& operator=(const ReadTextureJob& other) noexcept = delete;

  ~ReadTextureJob() override = default;

  void Work() override;

 private:
  FileBuffer* file_buffer_;
  std::string file_path_{};
};

class DecompressTextureJob final : public Job {
 public:
  DecompressTextureJob() = default;
  DecompressTextureJob(FileBuffer* file_buffer_, bool is_uv_inverted, TextureBuffer* texture_buffer)
      : Job(JobType::ImageFileDecompressing),
        file_buffer_(file_buffer_),
        is_uv_inverted(is_uv_inverted),
		texture_buffer_(texture_buffer){}

  void DecompressTexture(unsigned char* data);

  DecompressTextureJob(DecompressTextureJob&& other) noexcept = default;
  DecompressTextureJob& operator=(DecompressTextureJob&& other) noexcept =
      default;
  DecompressTextureJob(const DecompressTextureJob& other) noexcept = delete;
  DecompressTextureJob& operator=(const DecompressTextureJob& other) noexcept =
      delete;

  ~DecompressTextureJob() override = default;

  void Work() override;

 private:
  FileBuffer* file_buffer_;
  bool is_uv_inverted = false;
  TextureBuffer* texture_buffer_;
};


class TextureToGPUJob final : public Job {
 public:
  TextureToGPUJob() = default;
  TextureToGPUJob(TextureBuffer* texture_buffer)
      : Job(JobType::TextureToGPU), texture_buffer_(texture_buffer) {}

  void TextureToGPU();

  TextureToGPUJob(TextureToGPUJob&& other) noexcept = default;
  TextureToGPUJob& operator=(TextureToGPUJob&& other) noexcept =
      default;
  TextureToGPUJob(const TextureToGPUJob& other) noexcept = delete;
  TextureToGPUJob& operator=(const TextureToGPUJob& other) noexcept =
      delete;

  ~TextureToGPUJob() override = default;

  void Work() override;
  bool isRepeated = false;
  bool srgb_ = false;

 private:
  TextureBuffer* texture_buffer_;


};
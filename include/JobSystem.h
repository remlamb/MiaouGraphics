//#pragma once
//#include <functional>
//
//// A Dispatched job will receive this as function argument:
//struct JobDispatchArgs {
//  uint32_t jobIndex;
//  uint32_t groupIndex;
//};
//
//namespace Job {
//// Create the internal resources such as worker threads, etc. Call it once when
//// initializing the application.
//void Initialize();
//// Add a job to execute asynchronously. Any idle thread will execute this job.
//void Execute(const std::function<void()>& job);
//// Divide a job onto multiple jobs and execute in parallel.
////  jobCount    : how many jobs to generate for this task.
////  groupSize   : how many jobs to execute per thread. Jobs inside a group
////  execute serially. It might be worth to increase for small jobs func        :
////  receives a JobDispatchArgs as parameter
//void Dispatch(uint32_t jobCount, uint32_t groupSize,
//              const std::function<void(JobDispatchArgs)>& job);
//
//bool IsBusy();
//
//void Wait();
//}
//
//class JobSystem
//{
//	
//};
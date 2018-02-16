#pragma once
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

class WorkerThread
{
private:
	std::thread thread;
	std::queue<std::function<void()>>& jobs;
	std::mutex& jobQueueMutex;
	bool isBusy = false;
	bool isPaused = false;
	bool shouldFinish = false;
public:
	WorkerThread(std::queue<std::function<void()>>& jobQueue, std::mutex& jobQueueMutex);
	WorkerThread(WorkerThread&& other) = default;
	~WorkerThread();
	void Start();
	void Join();
	bool IsBusy() const;
	void SetBusy(bool busy);
};

class JobScheduler
{
private:
	unsigned int workerCount;
	std::vector<WorkerThread> workerThreads;
	std::queue<std::function<void()>> jobs;
	std::mutex jobQueueMutex;
public:
	JobScheduler(unsigned int workerCount = 6);
	~JobScheduler();
	void Init();
	void Shutdown();
	void WaitForCompletion();
	void ScheduleJob(std::function<void()> job);
	unsigned int GetWorkerCount() const;
};
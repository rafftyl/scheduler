#include "JobSystem.h"
#include <algorithm>
#include <iostream>

WorkerThread::WorkerThread(std::queue<std::function<void()>>& jobQueue, std::mutex& jobQueueMutex) : jobs(jobQueue), jobQueueMutex(jobQueueMutex)
{
}

WorkerThread::~WorkerThread()
{
	Join();	
}

void WorkerThread::Start()
{
	auto workerThreadFunction = [this]()
	{
		while (!shouldFinish)
		{
			if (!isPaused)
			{
				jobQueueMutex.lock();
				std::function<void()> job;
				if (jobs.size() > 0)
				{
					job = jobs.front();
					jobs.pop();
				}
				
				if (job != nullptr)
				{
					SetBusy(true);
					jobQueueMutex.unlock();
					job();
					SetBusy(false);
				}
				else
				{
					jobQueueMutex.unlock();
				}
			}
		}
	};

	thread = std::thread(workerThreadFunction);
}

void WorkerThread::Join()
{
	if (thread.joinable())
	{
		shouldFinish = true;
		thread.join();
	}
}

bool WorkerThread::IsBusy() const
{
	return isBusy;
}

void WorkerThread::SetBusy(bool busy)
{
	isBusy = busy;
}

JobScheduler::JobScheduler(unsigned int workerCount) : workerCount(workerCount)
{
	for (unsigned int i = 0; i < workerCount; ++i)
	{
		workerThreads.push_back(WorkerThread(jobs, jobQueueMutex));
	}
}

JobScheduler::~JobScheduler()
{
	for (auto& worker : workerThreads)
	{
		worker.Join();
	}
}

void JobScheduler::Init()
{
	for (auto& worker : workerThreads)
	{
		worker.Start();
	}
}

void JobScheduler::Shutdown()
{
	for (auto& worker : workerThreads)
	{
		worker.Join();
	}
}

void JobScheduler::WaitForCompletion()
{
	while (true)
	{
		if (jobQueueMutex.try_lock())
		{
			size_t jobsQueued = jobs.size();
			jobQueueMutex.unlock();

			if (jobsQueued == 0)
			{
				auto iter = std::find_if(workerThreads.begin(), workerThreads.end(),
					[](const WorkerThread& worker) {return worker.IsBusy();});
				if (iter == workerThreads.end())
				{
					return;
				}
			}
		}
	}
}

void JobScheduler::ScheduleJob(std::function<void()> job)
{
	std::scoped_lock<std::mutex> lock(jobQueueMutex);
	jobs.push(job);
}

unsigned int JobScheduler::GetWorkerCount() const
{
	return workerCount;
}


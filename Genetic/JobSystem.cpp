#include "JobSystem.h"
#include <algorithm>

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
			if (!isPaused && this->jobQueueMutex.try_lock())
			{
				std::function<void()> job;
				if (jobs.size() > 0)
				{
					job = jobs.front();
					jobs.pop();
				}
				this->jobQueueMutex.unlock();

				if (job != nullptr)
				{
					SetBusy(true);
					job();
					SetBusy(false);
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

void WorkerThread::SetPaused(bool pause)
{
	isPaused = false;
}

JobScheduler::JobScheduler(int workerCount)
{
	for (int i = 0; i < workerCount; ++i)
	{
		workerThreads.push_back(WorkerThread(jobs, jobQueueMutex));
	}
}

JobScheduler::~JobScheduler()
{
}

void JobScheduler::Init()
{
	for (auto& worker : workerThreads)
	{
		worker.Start();
	}
}

void JobScheduler::WaitForCompletion() const
{
	while (std::find_if(workerThreads.begin(), workerThreads.end(), [](const WorkerThread& worker) {return worker.IsBusy();}) != workerThreads.end());
}

void JobScheduler::ScheduleJob(std::function<void()> job)
{
	std::scoped_lock<std::mutex> lock(jobQueueMutex);
	jobs.push(job);
}

void JobScheduler::SetPaused(bool pause)
{
	for (auto& thread : workerThreads)
	{
		thread.SetPaused(pause);
	}
}

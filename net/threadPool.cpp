#include "threadPool.hpp"

const int MaxTaskCount = 100;

namespace Network {

	ThreadPool::Object_Creator ThreadPool::object_creator_;

	ThreadPool::ThreadPool() : syQueue_(MaxTaskCount)
	{
		int numThreads = std::thread::hardware_concurrency();
		Start(numThreads);
	}

	ThreadPool::~ThreadPool(void)
	{
		Stop();
	}

	void ThreadPool::Start(int numThreads)
	{
		running_ = true;
		std::cout << "ThreadPool start" << std::endl;
		for (int i = 0; i < numThreads; ++i)
		{
			threadgroup_.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
		}
	}

	void ThreadPool::Stop()
	{
		std::call_once(flag_, [this] {StopThreadGroup(); }); 
	}

	void ThreadPool::AddTask(const TaskInSyncQueue& task)
	{
		syQueue_.Put(task);
	}

	void ThreadPool::RunInThread()
	{
		while (running_)
		{
			std::list<TaskInSyncQueue> list;
			syQueue_.Take(list);

			for (auto& task : list)
			{
				if (!running_)
					return;
				
				task.processTask();
			}
		}
	}

	void ThreadPool::StopThreadGroup()
	{
		syQueue_.Stop(); 
		running_ = false;

		for (auto thread : threadgroup_)
		{
			if (thread)
				thread->join();
		}
		threadgroup_.clear();
	}

}

#ifndef _KVSTORESERVER_THREADPOOL_HPP
#define _KVSTORESERVER_THREADPOOL_HPP

#include <vector>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#include"syncQueue.hpp"

namespace KvStoreServer {

	class ThreadPool
	{
	public:
		ThreadPool();
		~ThreadPool();
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator =(const ThreadPool&) = delete;

		void Start(int numThreads);
		void Stop();
		void AddTask(const TaskInSyncQueue& task);

	private:
		void RunInThread();
		void StopThreadGroup();

		std::list<std::shared_ptr<std::thread>> threadgroup_; 
		SyncQueue syQueue_; 
		std::atomic_bool running_;
		std::once_flag flag_;
	};
}

#endif //_KVSTORESERVER_THREADPOOL_HPP

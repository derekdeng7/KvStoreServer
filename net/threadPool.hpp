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
		static ThreadPool* getInstance()
		{
			static ThreadPool instance;
			return &instance;
		}

		void Start(int numThreads);
		void Stop();
		void AddTask(const TaskInSyncQueue& task);

	private:
		struct  Object_Creator
		{
			Object_Creator()
			{
				ThreadPool::getInstance();
			};
		};
		static Object_Creator object_creator_;

		ThreadPool();
		~ThreadPool();
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator =(const ThreadPool&) = delete;
		void RunInThread();
		void StopThreadGroup();

		std::list<std::shared_ptr<std::thread>> threadgroup_; 
		SyncQueue syQueue_; 
		std::atomic_bool running_;
		std::once_flag flag_;
	};
}

#endif //_KVSTORESERVER_THREADPOOL_HPP

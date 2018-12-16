#ifndef _NETWORK_QYNCQUEUE_HPP
#define _NETWORK_QYNCQUEUE_HPP

#include <iostream>
#include<list>
#include <map>
#include<mutex>
#include<thread>
#include<condition_variable>
#include <iostream>

#include "task.hpp"

namespace Network {

	class SyncQueue
	{
	public:
		SyncQueue(int MaxTaskCount);
		~SyncQueue();
		SyncQueue(const SyncQueue&);
		SyncQueue& operator=(const SyncQueue&);
	
		void Put(const TaskInSyncQueue& task);
		void Take(std::list<TaskInSyncQueue>& list);
		void Take(TaskInSyncQueue& task);
		void Stop();
		bool Empty();
		bool Full();
		size_t Size();
		int Count();

	private:
		bool NotFull() const;
		bool NotEmpty() const;
		void Add(const TaskInSyncQueue& task);

    	std::list<TaskInSyncQueue> queue_; 
    	std::mutex mutex_; 
    	std::condition_variable notEmpty_;
    	std::condition_variable notFull_; 
    	int maxSize_; 

    	bool needStop_; 
	};	

}

#endif //_NETWORK_QYNCQUEUE_HPP
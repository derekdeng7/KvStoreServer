#ifndef _KVSTORESERVER_NET_THREADPOOL_HPP
#define _KVSTORESERVER_NET_THREADPOOL_HPP

#include "syncQueue.hpp"
#include "../db/lsmTree.hpp"

#include <vector>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

namespace KvStoreServer {

    template<class T>
    class ThreadPool
    {
    public:
        ThreadPool()
        : syQueue_(),
          lsmTree_(nullptr),
		      running_(true)
        {}

        ~ThreadPool()
        { 
		        Stop();
        }

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator =(const ThreadPool&) = delete;

        void Start(int numThreads)
	      {
            lsmTree_ = std::make_shared<LSMTree>();
            lsmTree_->Start();

            getCallback_ = std::bind(&LSMTree::Get, lsmTree_, std::placeholders::_1, std::placeholders::_2);
            putCallback_ = std::bind(&LSMTree::Put, lsmTree_, std::placeholders::_1);

		        std::cout << "ThreadPool start" << std::endl;
		        for (int i = 0; i < numThreads; ++i)
		        {
			          threadgroup_.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
		        } 
        }

        void Stop()
        {
            std::call_once(flag_, [this] {StopThreadGroup(); }); 
        }

        void AddTask(const T& task)
        {
            syQueue_.Put(task);
        }

    private:
        void RunInThread()
        {
            while (running_)
            {
                std::list<T> list;
                syQueue_.Take(list);

                for(auto& task : list)
                {
                    if (!running_)
                        return;
              
                    task.SetCallback(getCallback_, putCallback_);
                    task.processTask();
                }
            } 
        }

        void StopThreadGroup()
        {
            syQueue_.Stop(); 
            running_ = false;

            for (auto thread : threadgroup_)
            {
              if(thread)
                thread->join();
            }
            threadgroup_.clear();
        }

        std::list<std::shared_ptr<std::thread>> threadgroup_; 
        SyncQueue<T> syQueue_; 
        std::shared_ptr<LSMTree> lsmTree_;
        std::atomic_bool running_;
        std::once_flag flag_;

        GetCallback getCallback_;
        PutCallback putCallback_;
    };
}

#endif //_KVSTORESERVER_NET_THREADPOOL_HPP

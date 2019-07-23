#ifndef _KVSTORESERVER_THREADGUARD_HPP
#define _KVSTORESERVER_THREADGUARD_HPP

#include"syncQueue.hpp"

#include <vector>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

namespace KvStoreServer {

    template<class T>
    class SyncThread
    {
    public:
        SyncThread()
        : syQueue_(),
		      running_(true)
        {}

        virtual ~SyncThread()
        { 
		        Stop();
        }

        SyncThread(const SyncThread&) = delete;
        SyncThread& operator =(const SyncThread&) = delete;

        void StartThread()
	      {
		        std::cout << "SyncThread start" << std::endl;
			      thread_ = std::make_shared<std::thread>(&SyncThread::RunInThread, this);
        }

        void Stop()
        {
            std::call_once(flag_, [this]
                {
                    syQueue_.Stop();
                    running_ = false;
                    if(thread_)
                    {
                        thread_->join();
                    }
                }
            );
        }

        void AddTask(const T& task)
        {
            syQueue_.Put(task);
        }

        virtual void ProcessTask(const T& task) = 0;

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
              
                    ProcessTask(task);
                }
            } 
        }

        SyncQueue<T> syQueue_; 
        std::atomic_bool running_;
        std::once_flag flag_;
        std::shared_ptr<std::thread> thread_;
    };
   
}

#endif //_KVSTORESERVER_THREADGUARD_HPP

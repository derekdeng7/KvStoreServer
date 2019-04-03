#ifndef _KVSTORESERVER_DB_SYNCQUEUE_HPP_
#define _KVSTORESERVER_DB_SYNCQUEUE_HPP_

#include "base.hpp"

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>

namespace KvStoreServer{

    template<typename T>
    class SyncQueue
    {
    public:
        SyncQueue(int maxSize = MAXSYNCQUEUESIZE) :maxSize_(maxSize), needStop_(false)
        {}

        void Put(const T&x)
        {
            Add(x);
        }

        void Put(T&&x)
        {
            Add(std::forward<T>(x));
        }

        void Take(std::list<T>& list)
        {
            std::unique_lock<std::mutex> locker(mutex_);
            notEmpty_.wait(locker, [this]{return needStop_ || NotEmpty(); });
            
            if(needStop_)
                return;
            list = std::move(queue_);
            notFull_.notify_one();
        }

        void Take(T& t)
        {
            std::unique_lock<std::mutex> locker(mutex_);
            notEmpty_.wait(locker, [this]{return needStop_ || NotEmpty(); });
            
            if(needStop_)
                return;
            t = queue_.front();
            queue_.pop_front();
            notFull_.notify_one();
        }

        void Stop()
        {
            {
                std::lock_guard<std::mutex> locker(mutex_);
                needStop_ = true;
            }
            notFull_.notify_all();
            notEmpty_.notify_all();
        }

        bool Empty()
        {
            std::lock_guard<std::mutex> locker(mutex_);
            return queue_.empty();
        }

        bool Full()
        {
            std::lock_guard<std::mutex> locker(mutex_);
            return queue_.size() == maxSize_;
        }

        size_t Size()
        {
            std::lock_guard<std::mutex> locker(mutex_);
            return queue_.size();
        }

        int Count()
        {
            return queue_.size();
        }

    private:
        bool NotFull() const
        {
            bool full = queue_.size() >= maxSize_;
            if(full)
            {
                //std::cout << "full, waiting，thread id: " << std::this_thread::get_id() << std::endl;
            }
            return !full;
        }

        bool NotEmpty() const
        {
            bool empty = queue_.empty();
            if(empty)
            {
                //std::cout << "empty, waiting，thread id: " << std::this_thread::get_id() << std::endl;
            }
            return !empty;
        }

        template<typename F>
        void Add(F&&x)
        {
            std::unique_lock< std::mutex> locker(mutex_);
            notFull_.wait(locker, [this]{return needStop_ || NotFull(); });
            if(needStop_)
                return;

            queue_.push_back(std::forward<F>(x));
            notEmpty_.notify_one();
        }

    private:
        std::list<T> queue_;
        std::mutex mutex_; 
        std::condition_variable notEmpty_;
        std::condition_variable notFull_;
        size_t maxSize_; 
        bool needStop_;
    };

}

#endif //_KVSTORESERVER_DB_SYNCQUEUE_HPP_

#include "eventLoop.hpp"
#include "memory.h"

#include <unistd.h>

namespace KvStoreServer{

    EventLoop::EventLoop(size_t threadNum)
       :quit_(false),
        eventfd_(CreateEventfd()),
        threadid_(std::hash<std::thread::id>{}(std::this_thread::get_id())),
        threadNum_(threadNum),
        threadPool_(nullptr),
        epoller_(new Epoll()),
        wakeupfdChannel_(nullptr)
    {}
    
    EventLoop::~EventLoop()
    {
        wakeupfdChannel_->RemoveChannel();
        close(eventfd_);
    }

    void EventLoop::Start()
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        wakeupfdChannel_.reset(new Channel(eventfd_, shared_from_this()));
        wakeupfdChannel_->SetReadCallback(
            std::bind(&EventLoop::HandleRead, this)
        );
        wakeupfdChannel_->AddChannel();

        threadPool_ = std::make_shared<ThreadPool<TaskInSyncQueue>>(threadNum_);
        threadPool_->Start();
    }

    void EventLoop::Close()
    {
        std::cout << "threadPool_.use_count: " << threadPool_.use_count() << std::endl;
        threadPool_->Stop();

        quit_ = true;
        if(!isInLoopThread())
        {
            WakeUp();
        }
    }

    void EventLoop::Loop()
    {
        while(!quit_)
        {
            std::vector<Channel*> channels;
            epoller_->DoEpoll(&channels);

            for(auto it = channels.begin(); it != channels.end(); ++it)
            {
                (*it)->HandleEvent(); 
            }

            DoPendingFunctors();
        }
    }

    void EventLoop::AddChannel(Channel* channel)
    {
        epoller_->AddChannel(channel);
    }

    void EventLoop::RemoveChannel(Channel* channel)
    {
        epoller_->RemoveChannel(channel);
    }
    
    void EventLoop::Updatechannel(Channel* channel)
    {
        epoller_->UpdateChannel(channel);
    }

    void EventLoop::queueInLoop(EventCallback cb)
    {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            pendingFunctors_.push_back(std::move(cb));
        }
        
        if(!isInLoopThread() || callingPendingFunctors_)
        {
            WakeUp();
        }     
    }

    void EventLoop::runInLoop(EventCallback cb)
    {
        if(isInLoopThread())
        {
            cb();
        }
        else
        {
            queueInLoop(std::move(cb));
        }
    }

    bool EventLoop::isInLoopThread()
    {
        return threadid_ == std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

    void EventLoop::HandleRead()
    {
        uint64_t one = 1;
        ssize_t n = write(eventfd_, &one, sizeof(one));
        if(n != sizeof(one))
        {
            std::cout << "EventLoop::HandleReading() reads " << n << " bytes instead of 8" << std::endl;
        } 
    }

    void EventLoop::AddTask(const TaskInSyncQueue& task)
    {
        threadPool_->AddTask(task);
    }

    size_t EventLoop::GetThreadNum() const
    {
        return threadNum_;
    }

    void EventLoop::WakeUp()
    {
        uint64_t one = 1;
        ssize_t n = write(eventfd_, &one, sizeof(one));
        if(n != sizeof(one))
        {
            std::cout << "EventLoop::WakeUp() writes " << n << " bytes instead of 8" << std::endl;
        }
    }

    int EventLoop::CreateEventfd()
    {
        int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if(fd < 0)
        {
            perror("fail to create eventfd");
        }
        return fd;
    }

    void EventLoop::DoPendingFunctors()
    {
        std::vector<EventCallback> tempVec;
        callingPendingFunctors_ = true;

        {
            std::unique_lock<std::mutex> locker(mutex_);
            tempVec.swap(pendingFunctors_);
        }
        
        for(auto it = tempVec.begin(); it != tempVec.end(); it++)
        {
            (*it)();
        }
        callingPendingFunctors_ = false;
    }
}

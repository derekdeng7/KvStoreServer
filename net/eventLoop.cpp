#include "channel.hpp"
#include "epoll.hpp"
#include "eventLoop.hpp"
#include "timerQueue.hpp"
#include "timeStamp.hpp"
#include "memory.h"

#include <unistd.h>

namespace KvStoreServer{

    EventLoop::EventLoop()
       :quit_(false),
        eventfd_(CreateEventfd()),
        threadid_(std::hash<std::thread::id>{}(std::this_thread::get_id())),
        epoller_(new Epoll()),
        timerQueue_(nullptr),
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

        timerQueue_.reset(new TimerQueue(shared_from_this()));
    }

    void EventLoop::Close()
    {
        quit_ = true;
        if(!IsInLoopThread())
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

    void EventLoop::QueueInLoop(EventCallback cb)
    {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            pendingFunctors_.push_back(std::move(cb));
        }
        
        if(!IsInLoopThread() || callingPendingFunctors_)
        {
            WakeUp();
        }     
    }

    void EventLoop::RunInLoop(EventCallback cb)
    {
        if(IsInLoopThread())
        {
            cb();
        }
        else
        {
            QueueInLoop(std::move(cb));
        }
    }

    bool EventLoop::IsInLoopThread()
    {
        return threadid_ == std::hash<std::thread::id>{}(std::this_thread::get_id());
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

    TimerId EventLoop::RunAt(TimeStamp time, TimerCallback cb)
    {
        return timerQueue_->AddTimer(std::move(cb), time, 0.0);
    }

    TimerId EventLoop::RunAfter(double delay, TimerCallback cb)
    {
        TimeStamp time(TimeStamp::NowAfter(delay));
        return RunAt(std::move(time), std::move(cb));
    }

    TimerId EventLoop::RunEvery(double interval, TimerCallback cb)
    {
        TimeStamp time(TimeStamp::NowAfter(interval));
        return timerQueue_->AddTimer(std::move(cb), std::move(time), interval);
    }

    void EventLoop::CancelTimer(TimerId timerId)
    {
        return timerQueue_->CancelTimer(timerId);
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

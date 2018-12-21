#include "eventLoop.hpp"

namespace KvStoreServer{

    EventLoop::EventLoop()
       :quit_(false),
        epoller_(std::make_shared<Epoll>()),
        eventfd_(CreateEventfd()),
        threadid_(std::hash<std::thread::id>{}(std::this_thread::get_id())),
        wakeupfdChannel_(nullptr)
    {
        
    }
    
    EventLoop::~EventLoop()
    {
        wakeupfdChannel_->RemoveChannel();
        close(eventfd_);
    }

    void EventLoop::Start()
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        wakeupfdChannel_ = std::make_shared<Channel>(eventfd_, addr, shared_from_this());
        wakeupfdChannel_->SetReadCallback(
            std::bind(&EventLoop::HandleRead, shared_from_this())
        );
        wakeupfdChannel_->AddChannel();
    }

    void EventLoop::Close()
    {
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

    void EventLoop::queueInLoop(TaskInEventLoop& task)
    {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            pendingFunctors_.push_back(task);
        }
        
        if(!isInLoopThread() || callingPendingFunctors_)
        {
            WakeUp();
        }     
    }

    void EventLoop::runInLoop(TaskInEventLoop& task)
    {
        if(isInLoopThread())
        {
            task.processTask();
        }
        else
        {
            queueInLoop(task);
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
        std::vector<TaskInEventLoop> tempVec;
        callingPendingFunctors_ = true;

        {
            std::unique_lock<std::mutex> locker(mutex_);
            tempVec.swap(pendingFunctors_);
        }
        
        for(auto it = tempVec.begin(); it != tempVec.end(); it++)
        {
            (*it).processTask();
        }
        callingPendingFunctors_ = false;
    }
}
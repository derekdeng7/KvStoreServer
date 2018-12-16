#include "eventLoop.hpp"

namespace Network{

    EventLoop::EventLoop()
       :quit_(false),
        epoller_(new Epoll()),
        threadid_(std::hash<std::thread::id>{}(std::this_thread::get_id()))
    {
        eventfd_ = CreateEventfd();
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        eventfdChannel_ = new Channel(eventfd_, addr, this);
        eventfdChannel_->SetCallBack(this);
        eventfdChannel_->EnableReading();
    }
    
    EventLoop::~EventLoop()
    {}

    void EventLoop::Loop()
    {
        //std::cout << "EventLoop::Loop" << std::endl;
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
    
    void EventLoop::Update(Channel* channel)
    {
        //std::cout << "EventLoop::Update" << std::endl;
        epoller_->Update(channel);
    }

    void EventLoop::queueInLoop(TaskInEventLoop& task)
    {
        //std::cout << "EventLoop::queueLoop" << std::endl;
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

    void EventLoop::HandleReading()
    {
        //std::cout << "EventLoop::HandleReading" << std::endl;
        uint64_t one = 1;
        ssize_t n = write(eventfd_, &one, sizeof(one));
        if(n != sizeof(one))
        {
            std::cout << "EventLoop::HandleReading() reads " << n << " bytes instead of 8" << std::endl;
        } 
    }

    void EventLoop::HandleWriting()
    {}

    void EventLoop::WakeUp()
    {
        //std::cout << "EventLoop::WakeUp" << std::endl;
        uint64_t one = 1;
        ssize_t n = write(eventfd_, &one, sizeof(one));
        if(n != sizeof(one))
        {
            std::cout << "EventLoop::WakeUp() writes " << n << " bytes instead of 8" << std::endl;
        }
    }

    int EventLoop::CreateEventfd()
    {
        //std::cout << "EventLoop::CreateEventfd" << std::endl;
        int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if(fd < 0)
        {
            perror("fail to create eventfd");
        }
        return fd;
    }

    void EventLoop::DoPendingFunctors()
    {
        //std::cout << "EventLoop::DoPendingFunctors" << std::endl;
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
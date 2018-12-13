#include "eventLoop.hpp"

namespace Network{

    EventLoop::EventLoop()
       :quit_(false),
        epoller_(new Epoll())
    {}
    
    EventLoop::~EventLoop()
    {}

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
    }
    }
    
    void EventLoop::Update(Channel* channel)
    {
        epoller_->Update(channel);
    }
}
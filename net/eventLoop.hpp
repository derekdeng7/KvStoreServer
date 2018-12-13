#ifndef _NETWORK_EVENTLOOP_H
#define _NETWORK_EVENTLOOP_H

#include "channel.hpp"
#include "declear.hpp"
#include "epoll.hpp"

namespace Network{

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Update(Channel* channel);   

private:
    bool quit_;
    Epoll* epoller_;
};

}

#endif  //_NETWORK_EVENTLOOP_H
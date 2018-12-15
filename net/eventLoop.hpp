#ifndef _NETWORK_EVENTLOOP_H
#define _NETWORK_EVENTLOOP_H

#include <vector>
#include <sys/eventfd.h>
#include <netinet/in.h>

#include "channel.hpp"
#include "channelCallBack.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "epoll.hpp"

namespace Network{

class EventLoop : public ChannelCallBack
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Update(Channel* channel);
    void queueLoop(Connector* connector);

    void virtual HandleReading();
    void virtual HandleWriting();  

private:
    void WakeUp();
    int CreateEventfd();
    void DoPendingFunctors();

    bool quit_;
    Epoll* epoller_;
    int eventfd_;
    Channel* wakeupChannel_;
    std::vector<Connector*> pendingFunctors_;
};

}

#endif  //_NETWORK_EVENTLOOP_H
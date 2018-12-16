#ifndef _NETWORK_EVENTLOOP_H
#define _NETWORK_EVENTLOOP_H

#include <vector>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <mutex>
#include <thread>

#include "channel.hpp"
#include "channelCallBack.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "epoll.hpp"
#include "task.hpp"

namespace Network{

class EventLoop : public ChannelCallBack
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Update(Channel* channel);
    void queueInLoop(TaskInEventLoop& task);
    void runInLoop(TaskInEventLoop& task);
    bool isInLoopThread();

    void virtual HandleReading();
    void virtual HandleWriting();  

private:
    void WakeUp();
    int CreateEventfd();
    void DoPendingFunctors();

    bool quit_;
    bool callingPendingFunctors_;
    Epoll* epoller_;
    int eventfd_;
    const size_t threadid_;
    std::mutex mutex_;
    Channel* eventfdChannel_;
    std::vector<TaskInEventLoop> pendingFunctors_;
};

}

#endif  //_NETWORK_EVENTLOOP_H
#ifndef _KVSTORESERVER_EVENTLOOP_H
#define _KVSTORESERVER_EVENTLOOP_H

#include <vector>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <mutex>
#include <thread>
#include <memory>

#include "channel.hpp"
#include "channelCallback.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "epoll.hpp"
#include "task.hpp"

namespace KvStoreServer{

class EventLoop : public ChannelCallback,
                  public std::enable_shared_from_this<EventLoop>
{
public:
    EventLoop();
    ~EventLoop();

    void Start();
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
    std::shared_ptr<Epoll> epoller_;
    int eventfd_;
    const size_t threadid_;
    std::mutex mutex_;
    std::shared_ptr<Channel> eventfdChannel_;
    std::vector<TaskInEventLoop> pendingFunctors_;
};

}

#endif  //_KVSTORESERVER_EVENTLOOP_H
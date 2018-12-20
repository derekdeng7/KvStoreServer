#ifndef _KVSTORESERVER_EVENTLOOP_HPP_
#define _KVSTORESERVER_EVENTLOOP_HPP_

#include <vector>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>

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
        void Close();
        void Loop();
        void AddChannel(Channel* channel);
        void RemoveChannel(Channel* channel);
        void Updatechannel(Channel* channel);
        void queueInLoop(TaskInEventLoop& task);
        void runInLoop(TaskInEventLoop& task);
        bool isInLoopThread();

        void virtual HandleReading();
        void virtual HandleWriting();  

    private:
        void WakeUp();
        int CreateEventfd();
        void DoPendingFunctors();

        std::atomic_bool quit_;
        bool callingPendingFunctors_;
        std::shared_ptr<Epoll> epoller_;
        int eventfd_;
        const size_t threadid_;
        std::mutex mutex_;
        std::shared_ptr<Channel> wakeupfdChannel_;
        std::vector<TaskInEventLoop> pendingFunctors_;
    };

}

#endif  //_KVSTORESERVER_EVENTLOOP_HPP_
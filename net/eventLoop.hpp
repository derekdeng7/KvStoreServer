#ifndef _KVSTORESERVER_NET_EVENTLOOP_HPP_
#define _KVSTORESERVER_NET_EVENTLOOP_HPP_

#include "declear.hpp"
#include "epoll.hpp"
#include "task.hpp"

#include <vector>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>

namespace KvStoreServer{

    class EventLoop : public std::enable_shared_from_this<EventLoop>
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

        void HandleRead();  

    private:
        void WakeUp();
        int CreateEventfd();
        void DoPendingFunctors();

        std::atomic_bool quit_;
        bool callingPendingFunctors_;
        int eventfd_;
        const size_t threadid_;
        std::unique_ptr<Epoll> epoller_;
        std::mutex mutex_;
        std::unique_ptr<Channel> wakeupfdChannel_;
        std::vector<TaskInEventLoop> pendingFunctors_;
    };

}

#endif  //_KVSTORESERVER_NET_EVENTLOOP_HPP_

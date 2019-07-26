#ifndef _KVSTORESERVER_NET_SERVER_HPP_
#define _KVSTORESERVER_NET_SERVER_HPP_

#include "declear.hpp"
#include "task.hpp"
#include "../include/base.hpp"
#include "../include/callback.hpp"
#include "../include/threadPool.hpp"

#include <sys/epoll.h>
#include <map>
#include <memory>

namespace KvStoreServer{

    class Server
    {
    public:
        Server(size_t threadNum, uint16_t port);
        ~Server();

        void Start();
        void Close();

        void RunAt(TimeStamp time, TimerCallback cb);
        void RunAfter(double delay, TimerCallback cb);
        void RunEvery(double interval, TimerCallback cb);

    private:
        void NewConnection(std::shared_ptr<Socket> socket);
        void Receive(int sockfd, const std::string& message);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void RemoveConnectionInLoop(int sockfd);
        void ClearConnections();

        size_t threadNum_;
        uint16_t port_;
        std::map<int, std::shared_ptr<Connector>> connections_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<Acceptor> acceptor_;
        std::shared_ptr<ThreadPool<TaskInSyncQueue>> threadPool_;
    };

}

#endif //_KVSTORESERVER_NET_SERVER_HPP_

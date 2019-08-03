#ifndef _KVSTORESERVER_NET_SERVER_HPP_
#define _KVSTORESERVER_NET_SERVER_HPP_

#include "declear.hpp"
#include "../include/callback.hpp"
#include "connector.hpp"

#include <sys/epoll.h>
#include <map>
#include <memory>
#include <queue>
#include <unordered_set>

namespace KvStoreServer{

    class Server
    {
    public:
        Server(uint16_t port, int timeoutSecond = 0);
        ~Server();

        void Start();
        void Close();
        void Send(int sockfd, const std::string& message);

        void RunAt(TimeStamp time, TimerCallback cb);
        void RunAfter(double delay, TimerCallback cb);
        void RunEvery(double interval, TimerCallback cb);

        void SetRecvCallback(RecvCallback callback)
        {
            recvCallback_ = callback;
        }

    private:
        void NewConnection(std::shared_ptr<Socket> socket);
        void Receive(int sockfd, const std::string& message);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void RemoveConnectionInLoop(int sockfd);
        void ClearConnections();

        void Timeout();

        size_t threadNum_;
        uint16_t port_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<Acceptor> acceptor_;
        RecvCallback recvCallback_;

        std::map<int, std::shared_ptr<Connector>> connections_;

        typedef std::unordered_set<std::shared_ptr<HeartBeat>> Bucket;
        std::queue<Bucket> connectionBuckets_;
        int timeoutSecond_;
        
    };

}

#endif //_KVSTORESERVER_NET_SERVER_HPP_

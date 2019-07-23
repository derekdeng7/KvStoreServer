#ifndef _KVSTORESERVER_NET_CLIENT_HPP_
#define _KVSTORESERVER_NET_CLIENT_HPP_

#include "address.hpp"
#include "declear.hpp"
#include "timeStamp.hpp"
#include "../include/callback.hpp"

#include <map>
#include <memory>
#include <thread>

namespace KvStoreServer{

    class Client
    {
    public:
        Client(size_t threadNum);
        ~Client();

        void Start();
        void Close();
        int Connect(const char* ip, uint16_t port);
        void Send(int sockfd, const std::string& message);

        void RunEvery(double interval, TimerCallback cb);

        void SetRecvCallback(RecvCallback callback)
        {
            recvCallback_ = callback;
        }

    private:
        void Receive(int sockfd, const std::string& message);
        void NewConnection(std::shared_ptr<Socket> socket);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void RemoveConnectionInLoop(int sockfd);
        void ClearConnections();

        std::shared_ptr<EventLoop> loop_;
        size_t threadNum_;
        RecvCallback recvCallback_;

        std::map<int, std::shared_ptr<Connector>> connections_;
    
    };
}

#endif // _KVSTORESERVER_NET_CLIENT_HPP_

#ifndef _KVSTORESERVER_NET_CLIENT_HPP_
#define _KVSTORESERVER_NET_CLIENT_HPP_

#include "socket.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "../include/timer.hpp"

namespace KvStoreServer{

    class Client
    {
    public:
        Client(const char* ip, uint16_t port, size_t threadNum);
        ~Client();

        void Start();
        void Close();
        void Send(int sockfd, const std::string& message);

    private:
        bool Connect(Socket& socket);
        void Receive(int sockfd, std::string& message);
        void NewConnection(int sockfd, const sockaddr_in& addr);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void ClearConnections();

        Address serverAddr_;
        std::shared_ptr<EventLoop> loop_;
        size_t threadNum_;

        std::map<int, std::shared_ptr<Connector>> connections_;
    
        std::map<int, size_t> counts_;
        std::string message_;

        Timer timer_;
        size_t sessions_;
        size_t finishSessions_;
        size_t maxCounts_;
        size_t messageSize_;
    };
}

#endif // _KVSTORESERVER_NET_CLIENT_HPP_

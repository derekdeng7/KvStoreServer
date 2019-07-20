#ifndef _KVSTORESERVER_NET_CLIENT_HPP_
#define _KVSTORESERVER_NET_CLIENT_HPP_

#include "socket.hpp"
#include "connector.hpp"
#include "declear.hpp"

namespace KvStoreServer{

    class Client
    {
    public:
        Client(const char* ip, uint16_t port, int nums);
        ~Client();

        void Start();
        void Close();
        void Send(const std::string& message);

    private:
        bool Connect(Socket& socket);
        void Receive(int sockfd, std::string& message);
        void NewConnection(int sockfd, const sockaddr_in& addr);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void ClearConnections();

        Address serverAddr_;
        std::shared_ptr<EventLoop> loop_;
        int fdNums_;
        std::map<int, std::shared_ptr<Connector>> connections_;
        std::map<int, int> counts_;
        std::string message_;
    };
}

#endif // _KVSTORESERVER_NET_CLIENT_HPP_

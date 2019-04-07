#ifndef _KVSTORESERVER_NET_SERVER_HPP_
#define _KVSTORESERVER_NET_SERVER_HPP_

#include "acceptor.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "../include/base.hpp"

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

    private:
        void NewConnection(int sockfd, const sockaddr_in& addr);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void ClearConnections();

        size_t threadNum_;
        uint16_t port_;
        std::map<int, std::shared_ptr<Connector>> connections_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<Acceptor> acceptor_;
    };

}

#endif //_KVSTORESERVER_NET_SERVER_HPP_

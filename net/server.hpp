#ifndef _KVSTORESERVER_SERVER_HPP_
#define _KVSTORESERVER_SERVER_HPP_

#include <sys/epoll.h>
#include <map>
#include <memory>

#include "acceptor.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "define.hpp"
#include "eventLoop.hpp"
#include "threadPool.hpp"

namespace KvStoreServer{

    class Server : public std::enable_shared_from_this<Server>
    {
    public:
        Server(uint16_t port);
        ~Server();

        void Start();
        void Close();
        void NewConnection(int sockfd, const sockaddr_in& addr);
        void WriteComplete();
        void CloseConnection(int sockfd);
        void ClearConnections();

    private:
        uint16_t port_;
        std::map<int, std::shared_ptr<Connector>> connections_;
        std::shared_ptr<Acceptor> acceptor_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<ThreadPool> threadPool_;
    };

}

#endif //_KVSTORESERVER_SERVER_HPP_

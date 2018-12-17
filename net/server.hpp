#ifndef _KVSTORESERVER_SERVER_HPP_
#define _KVSTORESERVER_SERVER_HPP_

#include <sys/epoll.h>
#include <map>
#include <memory>

#include "acceptor.hpp"
#include "connector.hpp"
#include "declear.hpp"
#include "define.hpp"
#include "socket.hpp"
#include "eventLoop.hpp"

namespace KvStoreServer{

class Server : public std::enable_shared_from_this<Server>
{
public:
    Server(uint16_t port);
    ~Server();

    void Start();
    void Stop();
    void NewConnection(int sockfd, sockaddr_in addr);

private:
    uint16_t port_;
    int sockfd_;
    epoll_event events_[MAX_EVENTS];
    std::map<int, std::shared_ptr<Connector>> connections_;
    std::shared_ptr<Acceptor> pAcceptor_;
    std::shared_ptr<EventLoop> loop_;
    
};

}

#endif //_KVSTORESERVER_SERVER_HPP_
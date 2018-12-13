#ifndef _NETWORK_SERVER_HPP_
#define _NETWORK_SERVER_HPP_

#include <sys/epoll.h>
#include <map>

#include "acceptor.hpp"
#include "connection.hpp"
#include "declear.hpp"
#include "define.hpp"
#include "socket.hpp"
#include "eventLoop.hpp"

namespace Network{

class Server
{
public:
    Server(uint16_t port);
    ~Server();

    void Start();
    void NewConnection(int sockfd, sockaddr_in addr);

private:
    uint16_t port_;
    int sockfd_;
    epoll_event events_[MAX_EVENTS];
    std::map<int, Connection*> connections_;
    Acceptor* pAcceptor_;
    EventLoop* loop_;
    
};

}

#endif //_NETWORK_SERVER_HPP_
#ifndef _NETWORK_ACCEPTOR_HPP_
#define _NETWORK_ACCEPTOR_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>

#include "channel.hpp"
#include "callBack.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "server.hpp"

namespace Network{

class Acceptor
{
public:
    Acceptor(EventLoop* loop, uint16_t port);
    ~Acceptor();

    void Start();
    void SetConnectCallBack(Server* pServerCallBack);
    void HandleCallBack(int sockfd);

private:
    int InitListenfd();
    bool SetNonBlocking(int fd);

    Socket* socket_;
    uint16_t port_;
    int listenfd_;
    Channel* pAcceptChannel_;
    Server* pServerCallBack_;
    EventLoop* loop_; 
};
}

#endif  //_NETWORK_ACCEPTOR_HPP_
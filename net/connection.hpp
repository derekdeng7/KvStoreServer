#ifndef _NETWORK_CONNECTION_HPP_
#define _NETWORK_CONNECTION_HPP_

#include <unistd.h>

#include "channel.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace Network{

class Connection
{
public:
    Connection(int sockfd, sockaddr_in addr, EventLoop* loop);
    ~Connection();

    void HandleCallBack(int sockfd);

private:
    int sockfd_;
    sockaddr_in addr_;
    Channel* pChannel_;
    EventLoop* loop_;
};
}

#endif // _NETWORK_CONNECTION_HPP_
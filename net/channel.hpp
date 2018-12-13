#ifndef _NETWORK_CHANNEL_HPP_
#define _NETWORK_CHANNEL_HPP_

#include <sys/epoll.h>
#include <iostream>

#include "acceptor.hpp"
#include "callBack.hpp"
#include "channel.hpp"
#include "connection.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace Network{

class Channel{

public:
    Channel(int sockfd, sockaddr_in addr, EventLoop* loop);
    ~Channel();
    void SetAcceptCallBack(Acceptor* acceptorCallBack);
    void SetConnectCallBack(Connection* connectCallBack);
    void HandleEvent();
    void SetRevents(int revent);
    void EnableReading();
    int GetEvents();
    int GetSockfd();

private:
    void Update();
    int sockfd_;
    int event_;
    int revent_;
    sockaddr_in addr_;
    Acceptor* acceptorCallBack_;
    Connection* connectCallBack_;
    EventLoop* loop_;

};

}

#endif //_NETWORK_CHANNEL_HPP_
#ifndef _NETWORK_CHANNEL_HPP_
#define _NETWORK_CHANNEL_HPP_

#include <sys/epoll.h>
#include <iostream>

#include "channel.hpp"
#include "channelCallBack.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace Network{

class Channel{

public:
    Channel(int sockfd, sockaddr_in addr, EventLoop* loop);
    ~Channel();
    void SetCallBack(ChannelCallBack* pCallBack);
    void HandleEvent();
    void SetRevents(int revent);
    void SetIsNewFlag();
    void EnableReading();
    void EnableWriting();
    void DisableWriting();
    bool IsWriting() const;
    bool IsNewChannel() const;
    int GetEvents() const;
    int GetSockfd() const;

private:
    void Update();
    int sockfd_;
    int event_;
    int revent_;
    bool isNew_;
    sockaddr_in addr_;
    ChannelCallBack* pCallBack_;
    EventLoop* loop_;

};

}

#endif //_NETWORK_CHANNEL_HPP_
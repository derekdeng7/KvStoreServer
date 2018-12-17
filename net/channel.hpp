#ifndef _KVSTORESERVER_CHANNEL_HPP_
#define _KVSTORESERVER_CHANNEL_HPP_

#include <sys/epoll.h>
#include <iostream>

#include "channel.hpp"
#include "channelCallback.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace KvStoreServer{

class Channel{

public:
    Channel(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop);
    ~Channel();
    void SetCallback(std::shared_ptr<ChannelCallback> pCallback);
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
    std::shared_ptr<ChannelCallback> pCallback_;
    std::shared_ptr<EventLoop> loop_;

};

}

#endif //_KVSTORESERVER_CHANNEL_HPP_
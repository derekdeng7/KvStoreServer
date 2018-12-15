#ifndef _NETWORK_CONNECTOR_HPP_
#define _NETWORK_CONNECTOR_HPP_

#include <unistd.h>
#include <string>

#include "buffer.hpp"
#include "channel.hpp"
#include "channelCallBack.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace Network{

class Connector : ChannelCallBack
{
public:
    Connector(int sockfd, sockaddr_in addr, EventLoop* loop);
    ~Connector();

    void Send(std::string message);
    void WriteComplete();

    void virtual HandleReading();
    void virtual HandleWriting();
     

private:
    int sockfd_;
    sockaddr_in addr_;
    Channel* pChannel_;
    EventLoop* loop_;
    Buffer* recvBuf_;
    Buffer* sendBuf_;
};
}

#endif // _NETWORK_CONNECTOR_HPP_
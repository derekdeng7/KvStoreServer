#ifndef _KVSTORESERVER_CONNECTOR_HPP_
#define _KVSTORESERVER_CONNECTOR_HPP_

#include <unistd.h>
#include <string>
#include <memory>

#include "buffer.hpp"
#include "channel.hpp"
#include "channelCallback.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace KvStoreServer{

    class Connector : public ChannelCallback,
                      public std::enable_shared_from_this<Connector>
    {
    public:
        Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop);
        ~Connector();

        void Start();
        void Send(const std::string& message);
        void SendInLoop(const std::string& message);
        void WriteComplete();

        void virtual HandleReading();
        void virtual HandleWriting();

    private:
        int sockfd_;
        sockaddr_in addr_;
        std::shared_ptr<Channel> pChannel_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<Buffer> recvBuf_;
        std::shared_ptr<Buffer> sendBuf_;
    };
}

#endif // _KVSTORESERVER_CONNECTOR_HPP_
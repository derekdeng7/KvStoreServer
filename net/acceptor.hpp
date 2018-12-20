#ifndef _KVSTORESERVER_ACCEPTOR_HPP_
#define _KVSTORESERVER_ACCEPTOR_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <memory>

#include "channel.hpp"
#include "channelCallback.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "server.hpp"

namespace KvStoreServer{

    class Acceptor : public ChannelCallback,
                     public std::enable_shared_from_this<Acceptor>
    {
    public:
        Acceptor(std::shared_ptr<EventLoop> loop, uint16_t port);
        ~Acceptor();

        void Start();
        void Close();
        void SetCallback(std::shared_ptr<Server> ServerCallback);
        void virtual HandleReading();
        void virtual HandleWriting();

    private:
        int InitListenfd();
        bool SetNonBlocking(int fd);

        std::shared_ptr<Socket> socket_;
        uint16_t port_;
        int listenfd_;
        std::shared_ptr<Channel> acceptChannel_;
        std::shared_ptr<Server> serverCallback_;
        std::shared_ptr<EventLoop> loop_; 
    };
}

#endif  //_KVSTORESERVER_ACCEPTOR_HPP_
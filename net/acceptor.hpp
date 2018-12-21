#ifndef _KVSTORESERVER_ACCEPTOR_HPP_
#define _KVSTORESERVER_ACCEPTOR_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <memory>

#include "callback.hpp"
#include "channel.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "server.hpp"

namespace KvStoreServer{

    class Acceptor : public std::enable_shared_from_this<Acceptor>
    {
    public:
        typedef std::function<void(int sockfd, const sockaddr_in& addr)> NewConnectionCallback;

        Acceptor(std::shared_ptr<EventLoop> loop, uint16_t port);
        ~Acceptor();

        void Start();
        void Close();
        void SetNewConnectionCallback(const NewConnectionCallback& callback);
        void HandleRead();

    private:
        int InitListenfd();
        bool SetNonBlocking(int fd);

        std::shared_ptr<Socket> socket_;
        uint16_t port_;
        int listenfd_;
        std::shared_ptr<Channel> acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        std::shared_ptr<EventLoop> loop_; 
    };
}

#endif  //_KVSTORESERVER_ACCEPTOR_HPP_
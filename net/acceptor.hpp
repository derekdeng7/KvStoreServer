#ifndef _KVSTORESERVER_ACCEPTOR_HPP_
#define _KVSTORESERVER_ACCEPTOR_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <memory>

#include "callback.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "socket.hpp"

namespace KvStoreServer{

    class Acceptor
    {
    public:

        Acceptor(std::shared_ptr<EventLoop> loop, uint16_t port);
        ~Acceptor();

        void Start();
        void Close();
        void SetNewConnectionCallback(const NewConnectionCallback& callback);
        void HandleRead();

    private:
        int InitListenfd();
        bool SetNonBlocking(int fd);

        std::unique_ptr<Socket> socket_;
        uint16_t port_;
        int listenfd_;
        std::unique_ptr<Channel> acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        std::shared_ptr<EventLoop> loop_; 
    };
}

#endif  //_KVSTORESERVER_ACCEPTOR_HPP_

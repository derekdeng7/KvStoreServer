#ifndef _KVSTORESERVER_CONNECTOR_HPP_
#define _KVSTORESERVER_CONNECTOR_HPP_

#include <unistd.h>
#include <string>
#include <memory>

#include "buffer.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

namespace KvStoreServer{

    class Connector : public std::enable_shared_from_this<Connector>
    {
    public:
        Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop, std::shared_ptr<ThreadPool> threadPool, std::shared_ptr<Server> server);
        ~Connector();

        void Start();
        void Close();
        void Send(const std::string& message);
        void SendInLoop(const std::string& message);

        void HandleRead();
        void HandleWrite();

        void SetWriteCompleteCallback(EventCallback callback);

    private:
        int sockfd_;
        sockaddr_in addr_;
        std::unique_ptr<Channel> channel_;
        std::unique_ptr<Buffer> recvBuf_;
        std::unique_ptr<Buffer> sendBuf_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<ThreadPool> threadPool_;
        std::shared_ptr<Server> server_;
        WriteCompleteCallback writeCompleteCallback_;
    };
}

#endif // _KVSTORESERVER_CONNECTOR_HPP_

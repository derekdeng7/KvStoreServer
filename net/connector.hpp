#ifndef _KVSTORESERVER_NET_CONNECTOR_HPP_
#define _KVSTORESERVER_NET_CONNECTOR_HPP_

#include "buffer.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "../include/threadPool.hpp"

#include <unistd.h>
#include <string>
#include <memory>

namespace KvStoreServer{

    class Connector : public std::enable_shared_from_this<Connector>
    {
    public:
        Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop, std::shared_ptr<ThreadPool<TaskInSyncQueue>> threadPool, std::shared_ptr<Server> server);
        ~Connector();

        void Start();
        void Close();
        void Send(const Message& message);
        void SendInLoop(const Message& message);

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
        std::shared_ptr<ThreadPool<TaskInSyncQueue>> threadPool_;
        std::shared_ptr<Server> server_;
        WriteCompleteCallback writeCompleteCallback_;
    };
}

#endif // _KVSTORESERVER_NET_CONNECTOR_HPP_

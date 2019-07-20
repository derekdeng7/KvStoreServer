#ifndef _KVSTORESERVER_NET_CONNECTOR_HPP_
#define _KVSTORESERVER_NET_CONNECTOR_HPP_

#include "buffer.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"
#include "../include/syncQueue.hpp"

#include <unistd.h>
#include <string>
#include <memory>

namespace KvStoreServer{

    class Connector : public std::enable_shared_from_this<Connector>
    {
    public:
        Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop);

        ~Connector();

        void Start();
        void Close();
        void Send(const std::string& message);
        void SendInLoop(const std::string& message);

        void HandleRead();
        void HandleWrite();

        void SetRecvCallback(RecvCallback callback)
        {
            recvCallback_ = callback;
        }

        void SetWriteCompleteCallback(WriteCompleteCallback callback)
        {
            writeCompleteCallback_ = callback;
        }

        void SetRemoveConnectionCallback(RemoveConnectionCallback callback)
        {
            removeConnectionCallback_ = callback;
        }

    private:
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;    
        std::unique_ptr<Buffer> recvBuf_;
        std::unique_ptr<Buffer> sendBuf_;
        std::shared_ptr<EventLoop> loop_;
        bool isMultiThread_;
        RecvCallback recvCallback_;
        RemoveConnectionCallback removeConnectionCallback_;
        WriteCompleteCallback writeCompleteCallback_;
    };
}

#endif // _KVSTORESERVER_NET_CONNECTOR_HPP_

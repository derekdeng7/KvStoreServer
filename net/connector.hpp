#ifndef _KVSTORESERVER_NET_CONNECTOR_HPP_
#define _KVSTORESERVER_NET_CONNECTOR_HPP_

#include "declear.hpp"
#include "../include/syncQueue.hpp"

#include <unistd.h>
#include <string>
#include <memory>

namespace KvStoreServer{

    class Connector
    {
    public:
        Connector(std::shared_ptr<Socket> socket_, std::shared_ptr<EventLoop> loop);

        ~Connector();

        void Start();
        void Close();
        void Send(const std::string& message);
        void SendInLoop(const std::string& message);

        void Shutdown();
        void ShutdownInLoop();

        void ForceClose();
        void ForceCloseInLoop();
        void ForceCloseWithDelay(double seconds);

        void HandleRead();
        void HandleWrite();
        void HandleClose();

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

        void SetHeartBeat(const std::weak_ptr<HeartBeat>& beat)
        {
            beat_ = beat;
        }

        std::weak_ptr<HeartBeat>  GetHeartBeat() const
        {
            return beat_;
        }

    private:
        std::shared_ptr<Socket> socket_;
        std::unique_ptr<Channel> connChannel_;    
        std::unique_ptr<Buffer> recvBuf_;
        std::unique_ptr<Buffer> sendBuf_;
        std::shared_ptr<EventLoop> loop_;
        std::weak_ptr<HeartBeat> beat_;
        RecvCallback recvCallback_;
        RemoveConnectionCallback removeConnectionCallback_;
        WriteCompleteCallback writeCompleteCallback_;
    };

    //time wheel
    struct HeartBeat
    {
        explicit HeartBeat(const std::weak_ptr<Connector>& weakConn) 
        : weakConn_(weakConn)
        {}

        ~HeartBeat()
       {
            auto conn = weakConn_.lock();
            if(conn)
            {
                //conn->Shutdown();
                conn->Send("Heart beat...");
            }
        }

        std::weak_ptr<Connector> weakConn_;
    };
}

#endif // _KVSTORESERVER_NET_CONNECTOR_HPP_

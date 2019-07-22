#ifndef _KVSTORESERVER_NET_CHANNEL_HPP_
#define _KVSTORESERVER_NET_CHANNEL_HPP_

#include "../include/callback.hpp"
#include "declear.hpp"
#include "eventLoop.hpp"

#include <sys/epoll.h>
#include <iostream>

namespace KvStoreServer{

    class Channel
    {
    public:
        Channel(int sockfd, std::shared_ptr<EventLoop> loop);
        ~Channel();

        void AddChannel();
        void RemoveChannel();
        void UpdateChannel();

        void HandleEvent();
        void SetRevents(int revent);
        void EnableReading();
        void DisableReading();
        void EnableWriting();
        void DisableWriting();
        void DisableAll();
        bool IsWriting() const;
        int GetEvents() const;
        int GetSockfd() const;

        void SetReadCallback(EventCallback callback);
        void SetWriteCallback(EventCallback callback);

    private:        
        int sockfd_;
        int event_;
        int revent_;
        EventCallback readCallback_;
        EventCallback writeCallback_;
        std::shared_ptr<EventLoop> loop_;

    };

}

#endif //_KVSTORESERVER_NET_CHANNEL_HPP_

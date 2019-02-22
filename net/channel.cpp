#include "channel.hpp"
#include "unistd.h"

namespace KvStoreServer{

    Channel::Channel(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop)
       :sockfd_(sockfd),
        event_(0),
        revent_(0),
        addr_(addr),
        loop_(loop)
    {}

    Channel::~Channel()
    {
        RemoveChannel();
        close(sockfd_);
    }

    void Channel::AddChannel()
    {
        event_ |= EPOLLIN;
        loop_->AddChannel(this);
    }

    void Channel::RemoveChannel()
    {
        DisableAll();
        loop_->RemoveChannel(this);
    }

    void Channel::UpdateChannel()
    {
        loop_->Updatechannel(this);
    }

    void Channel::HandleEvent()
    {
        if(revent_ & EPOLLIN)
        {
            readCallback_();
        }
        
        if(revent_ & EPOLLOUT)
        {
            writeCallback_();
        }
    }

    void Channel::SetRevents(int revent)
    {
        revent_ = revent;
    }

    void Channel::EnableReading()
    {
        event_ |= EPOLLIN;
        UpdateChannel();
    }

    void Channel::DisableReading()
    {
        event_ &= ~EPOLLIN;
        UpdateChannel();
    }

    void Channel::EnableWriting()
    {
        event_ |= EPOLLOUT;
        UpdateChannel();
    }

    void Channel::DisableWriting()
    {
        event_ &= ~EPOLLOUT;
        UpdateChannel();
    }

    void Channel::DisableAll()
    {
        event_ = 0;
        UpdateChannel();
    }

    bool Channel::IsWriting() const
    {
        return event_ & EPOLLOUT;
    }

    int Channel::GetEvents() const
    {
        return event_;
    }

    int Channel::GetSockfd() const
    {
        return sockfd_;
    }

    void Channel::SetReadCallback(EventCallback callback)
    {
        readCallback_ = callback;
    }

    void Channel::SetWriteCallback(EventCallback callback)
    {
        writeCallback_ = callback;
    }

}

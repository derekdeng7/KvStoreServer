#include "channel.hpp"

namespace KvStoreServer{

    Channel::Channel(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop)
       :sockfd_(sockfd),
        event_(0),
        revent_(0),
        addr_(addr),
        callback_(nullptr),
        loop_(loop)
    {}

    Channel::~Channel()
    {
        RemoveChannel();
        close(sockfd_);
    }

    void Channel::SetCallback(std::shared_ptr<ChannelCallback> callback)
    {
        callback_ = callback;
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
            callback_->HandleReading();
        }
        
        if(revent_ & EPOLLOUT)
        {
            callback_->HandleWriting();
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

}
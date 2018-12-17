#include "channel.hpp"

namespace KvStoreServer{

    Channel::Channel(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop)
       :sockfd_(sockfd),
        event_(0),
        revent_(0),
        isNew_(true),
        addr_(addr),
        pCallback_(nullptr),
        loop_(loop)
    {}

    Channel::~Channel()
    {}

    void Channel::SetCallback(std::shared_ptr<ChannelCallback> pCallback)
    {
        pCallback_ = pCallback;
    }

    void Channel::HandleEvent()
    {
        if(revent_ & EPOLLIN)
        {
            //std::cout << "Channel::HandleEvent EPOLLIN" << std::endl;
            pCallback_->HandleReading();
        }
        
        if(revent_ & EPOLLOUT)
        {
            //std::cout << "Channel::HandleEvent EPOLLOUT" << std::endl;
            pCallback_->HandleWriting();
        }
    }

    void Channel::SetRevents(int revent)
    {
        revent_ = revent;
    }

    void Channel::SetIsNewFlag()
    {
        isNew_ = false;
    }

    void Channel::EnableReading()
    {
        event_ |= EPOLLIN;
        Update();
    }

    void Channel::EnableWriting()
    {
        event_ |= EPOLLOUT;
        Update();
    }

    void Channel::DisableWriting()
    {
        event_ &= ~EPOLLOUT;
        Update();
    }

    bool Channel::IsWriting() const
    {
        return event_ & EPOLLOUT;
    }

    bool Channel::IsNewChannel() const
    {
        return isNew_;
    }

    int Channel::GetEvents() const
    {
        return event_;
    }

    int Channel::GetSockfd() const
    {
        return sockfd_;
    }

    void Channel::Update()
    {
        loop_->Update(this);
    }

}
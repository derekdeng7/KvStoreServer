#include "channel.hpp"

namespace Network{

    Channel::Channel(int sockfd, sockaddr_in addr, EventLoop* loop)
       :sockfd_(sockfd),
        event_(0),
        revent_(0),
        isNew_(true),
        addr_(addr),
        pCallBack_(NULL),
        loop_(loop)
    {}

    Channel::~Channel()
    {}

    void Channel::SetCallBack(ChannelCallBack* pCallBack)
    {
        pCallBack_ = pCallBack;
    }

    void Channel::HandleEvent()
    {
        if(revent_ & EPOLLIN)
        {
            //std::cout << "Channel::HandleEvent EPOLLIN" << std::endl;
            pCallBack_->HandleReading();
        }
        
        if(revent_ & EPOLLOUT)
        {
            std::cout << "Channel::HandleEvent EPOLLOUT" << std::endl;
            pCallBack_->HandleWriting();
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
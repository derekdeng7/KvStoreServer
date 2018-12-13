#include "channel.hpp"

namespace Network{

    Channel::Channel(int sockfd, sockaddr_in addr, EventLoop* loop)
       :sockfd_(sockfd),
        event_(0),
        revent_(0),
        addr_(addr),
        acceptorCallBack_(NULL),
        connectCallBack_(NULL),
        loop_(loop)
    {}

    Channel::~Channel()
    {}

    void Channel::SetAcceptCallBack(Acceptor* acceptorCallBack)
    {
        acceptorCallBack_ = acceptorCallBack;
    }

    void Channel::SetConnectCallBack(Connection* connectCallBack)
    {
        connectCallBack_ = connectCallBack;
    }

    void Channel::HandleEvent()
    {
        if(revent_ & EPOLLIN)
        {
            if((acceptorCallBack_ == NULL && connectCallBack_ == NULL) ||
                (acceptorCallBack_ != NULL && connectCallBack_ != NULL))
            {
                perror("fail to handle event from a error channel");
                return;
            }

            if(acceptorCallBack_ != NULL)
            {
                acceptorCallBack_->HandleCallBack(sockfd_);
            }
            else if(connectCallBack_ != NULL)
            {
                connectCallBack_->HandleCallBack(sockfd_);
            }
        }
    }

    void Channel::SetRevents(int revent)
    {
        revent_ = revent;
    }

    void Channel::EnableReading()
    {
        event_ |= EPOLLIN;
        Update();
    }

    int Channel::GetEvents()
    {
        return event_;
    }

    int Channel::GetSockfd()
    {
        return sockfd_;
    }

    void Channel::Update()
    {
        loop_->Update(this);
    }

}
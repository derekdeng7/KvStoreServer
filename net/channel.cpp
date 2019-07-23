#include "channel.hpp"
#include "eventLoop.hpp"
#include "unistd.h"

namespace KvStoreServer{

    Channel::Channel(int fd, std::shared_ptr<EventLoop> loop)
       :fd_(fd),
        event_(0),
        revent_(0),
        loop_(loop)
    {}

    Channel::~Channel()
    {
        RemoveChannel();
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

    int Channel::GetFd() const
    {
        return fd_;
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

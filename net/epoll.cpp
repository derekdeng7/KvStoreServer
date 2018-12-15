#include "epoll.hpp"

namespace Network{

    Epoll::Epoll()
	{
        if((epollfd_ = epoll_create(FD_SIZE)) <= 0)
        {
            std::string err = "epoll_create error, epollfd: " + std::to_string(epollfd_);
            perror(err.c_str());
        }
    }

    Epoll::~Epoll()
	{}

    void Epoll::DoEpoll(std::vector<Channel*>* pChannels)
	{
        int fds = ::epoll_wait(epollfd_, events_, MAX_EVENTS, -1);
        if(fds == -1)
        {
            std::string err = "epoll_wait error, errno: " + std::to_string(errno);
            perror(err.c_str());
            return;
        }

        for(auto i = 0; i < fds; i++)
        {
            Channel* pChannel = static_cast<Channel*>(events_[i].data.ptr);
            pChannel->SetRevents(events_[i].events);
            pChannels->push_back(pChannel);
        }
    }

    void Epoll::Update(Channel* pChannel)
	{
        struct epoll_event ev;
        ev.data.ptr = pChannel;
        ev.events = pChannel->GetEvents();
        int fd = pChannel->GetSockfd();

        if(pChannel->IsNewChannel())
        {
            pChannel->SetIsNewFlag();
            epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
        }
        else
        {
            epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
        }
        
    }
    
}
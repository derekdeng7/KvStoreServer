#include <iostream>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "epoll.hpp"

namespace Network{

    Epoll::Epoll(int listenfd):listenfd_(listenfd), fd_size(1024), 
        event_size(100), buffer_size(1024), timeout_(-1)
    {
        events_ = new epoll_event[event_size];
        buf = new char[buffer_size];
    }

    void Epoll::Start()
    {
        int ret;
        memset(buf, 0, buffer_size);

        epfd_ = epoll_create(fd_size);

        AddEvent(listenfd_, EPOLLIN);
        while(1)
        {
            ret = epoll_wait(epfd_, events_, event_size, timeout_);
            if(ret == -1)
            {
                perror("epoll_pwait");
                exit(1);
            }

            HandleEvents(ret, buf);
        }
        close(epfd_);
    }

    void Epoll::HandleEvents(int num, char *buf)
    {
        int i;
        int fd;
    
        for (i = 0; i < num; i++)
        {
            fd = events_[i].data.fd;
            if ((fd == listenfd_))
            {
                HandleAccept();
                continue;
            }    
            if (events_[i].events & EPOLLIN)
                DoRead(fd, buf, i);
            if (events_[i].events & EPOLLOUT)
                DoWrite(fd, buf);
        }
    }

    void Epoll::HandleAccept()
    {
        int clifd;
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));

        socklen_t  cliaddrlen;
        while((clifd = accept(listenfd_, (struct sockaddr*)&cliaddr, &cliaddrlen)) > 0)
        {
            SetNonBlocking(clifd);
            printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
            AddEvent(clifd, EPOLLIN | EPOLLET);
        }
        
        if (clifd == -1)
        {
            if (errno != EAGAIN && errno != ECONNABORTED 
                && errno != EPROTO && errno != EINTR)
                {
                    perror("accpet error:");
                }      
        }
    }

    void Epoll::DoRead(int fd, char *buf, int i)
    {
        int nread = 0;
        int n = 0;
        while((nread = read(fd, buf + n, buffer_size - 1)) > 0)
        {
            std::cout << "Read message is : " << buf << std::endl;
            n += nread;
        }
        if (nread == -1  && errno != EAGAIN)
        {
            perror("read error:");
        }
        
        ModifyEvent(fd, events_[i].events | EPOLLOUT);           
    }

    void Epoll::DoWrite(int fd, char *buf)
    {
        int nwrite, data_size = strlen(buf);
        int n = data_size;
        while(n > 0)
        {
            nwrite = write(fd, buf + data_size - n, n);
            if (nwrite < n) 
            {
                if (nwrite == -1 && errno != EAGAIN) 
                {
                    perror("write error");
                }
                break;
            }
            n -= nwrite;
 /*           
            if (nwrite == -1)
            {
                perror("write error:");
                close(fd);
                DeleteEvent(fd, EPOLLOUT);
            }
            else
            {
                std::cout << "Send message is : " << buf << std::endl;
                ModifyEvent(fd, EPOLLIN);
            }
            */
        }    
        memset(buf, 0, buffer_size);
        close(fd);
    }

    void Epoll::AddEvent(int fd, int state)
    {
        struct epoll_event ev;
        ev.events = state;
        ev.data.fd = fd;
        epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
    }

    void Epoll::DeleteEvent(int fd, int state)
    {
        struct epoll_event ev;
        ev.events = state;
        ev.data.fd = fd;
        epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ev);
    }

    void Epoll::ModifyEvent(int fd, int state)
    {
        struct epoll_event ev;
        ev.events = state;
        ev.data.fd = fd;
        epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
    }

    void Epoll::SetNonBlocking(int fd)
    {
        int opts;

        opts = fcntl(fd, F_GETFL);
        if(opts < 0) {
            perror("fcntl(F_GETFL)\n");
            exit(1);
        }
        opts = (opts | O_NONBLOCK);
        if(fcntl(fd, F_SETFL, opts) < 0) 
        {
            perror("fcntl(F_SETFL)\n");
            exit(1);
        }
    }
}
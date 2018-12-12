#ifndef _NETWORK_EPOLL_HPP_
#define _NETWORK_EPOLL_HPP_

#include <sys/epoll.h>

#include "socket.hpp"

namespace Network{

class Epoll
{
public:
    Epoll(int listenfd);

    void Start();
    void HandleEvents(int num, char *buf);
    void HandleAccept();
    void AddEvent(int fd, int state);
    void DeleteEvent(int fd, int state);
    void ModifyEvent(int fd, int state);
    void DoRead(int fd, char *buf, int i);
    void DoWrite(int fd, char *buf);
    void SetNonBlocking(int fd);

private:
    int epfd_;
    int listenfd_;
    int event_size;
    int fd_size;
    int timeout_;
    int buffer_size;

    char *buf;
    epoll_event *events_;
    
};

}

#endif //_NETWORK_EPOLL_HPP_
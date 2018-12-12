#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "socket.hpp"

namespace Network{
    
    Socket::Socket():fd_(-1) {}

    Socket::Socket(int fd):fd_(fd){}

    int Socket::Fd() const
    {
        return fd_;
    }

    bool Socket::Valid() const
    {
        return fd_ != -1;
    }

    bool Socket::Create()
    {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        return fd_ != -1;
    }

    bool Socket::Bind(uint16_t port)
    {
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);

        return 0 == bind(fd_, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    }

    bool Socket::Connet(const Address& addr)
    {
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = addr.Ip();
        servaddr.sin_port = htons(addr.Port());

        return 0 == connect(fd_, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    }

    bool Socket::Listen()
    {
        return 0 == listen(fd_, BACKLOG);
    }

    int Socket::Accept()
    {
        struct sockaddr_in cliaddr;
        int confd;

        memset(&cliaddr, 0, sizeof(cliaddr));
        socklen_t len = sizeof(cliaddr);
        confd = accept(fd_, (struct sockaddr*)&cliaddr, &len);
        return confd;
    }

    bool Socket::Close()
    {
        bool flag = true;
        if (fd_ != -1)
        {
            flag = !close(fd_);
            fd_ = -1;
        }
        return flag;
    }

    bool Socket::SetOption(int optname, int optval)
    {
        return 0 == setsockopt(fd_, SOL_SOCKET, optname, &optval, sizeof(optval));
    }

    bool Socket::GetOption(int optname, int* optval)
    {
        socklen_t len = static_cast<socklen_t>(sizeof(*optval));
        return 0 == getsockopt(fd_, SOL_SOCKET, optname, optval, &len);
    }

    bool Socket::SetReuseAddress()
    {
        return SetOption(SO_REUSEADDR, 1);
    }

    bool Socket::GetPeerName(Address &addr, int sock)
    {
        struct sockaddr_in peeraddr;
        socklen_t len = sizeof(peeraddr);

        memset(&peeraddr, 0, sizeof(peeraddr));
        if (!getpeername(sock, (struct sockaddr*)&peeraddr, &len))
        {
                addr = Address(ntohs(peeraddr.sin_port),
                                peeraddr.sin_addr.s_addr);
                return true;
        }
        return false;
    }

    bool Socket::GetSockName(Address* addr)
    {
        struct sockaddr_in localaddr;
        socklen_t len = sizeof(localaddr);

        memset(&localaddr, 0, sizeof(localaddr));
        if (!getsockname(fd_, (struct sockaddr*)&localaddr, &len))
        {
                *addr = Address(ntohs(localaddr.sin_port),
                                localaddr.sin_addr.s_addr);
                return true;
        }
        return false;
    }

    bool Socket::AddFlag(int flag)
    {
        int cmd = fcntl(fd_, F_GETFL, 0);
        assert(cmd != -1);
        cmd |= flag;
        return -1 != fcntl(fd_, F_SETFL, flag);
    }

    bool Socket::SetNonBlock()
    {
        return AddFlag(O_NONBLOCK);
    }
}
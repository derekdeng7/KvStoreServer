#ifndef _KVSTORESERVER_NET_SOCKET_HPP_
#define _KVSTORESERVER_NET_SOCKET_HPP_

#include "address.hpp"
#include "declear.hpp"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

namespace KvStoreServer{

    class Socket
    {   
    public:
        Socket();
        Socket(int fd);
        Socket(int fd, sockaddr_in addr);

        int Fd() const;
        sockaddr_in ServerAddr() const;
        bool Valid() const;

        bool Create();
        bool Bind(uint16_t port);
        bool Connect(const Address& addr);
        bool Listen();
        int Accept();
        bool Close();

        bool SetOption(int value, int optval);
        bool GetOption(int value, int* optval);
        bool SetReuseAddress();
        bool GetPeerName(Address &addr, int sock);
        bool GetSockName(Address* addr);
        bool AddFlag(int flag);
        bool SetNonBlock();

    private:
        int fd_;
        struct sockaddr_in addr_;
        static const int BACKLOG = 50;
    };

}

#endif //_KVSTORESERVER_NET_SOCKET_HPP_

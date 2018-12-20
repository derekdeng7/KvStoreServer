#ifndef _KVSTORESERVER_SOCKET_HPP_
#define _KVSTORESERVER_SOCKET_HPP_

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "address.hpp"
#include "declear.hpp"

namespace KvStoreServer{

    class Socket
    {   
    public:
        Socket();
        Socket(int fd);

        int Fd() const;
        sockaddr_in Serveraddr() const;
        bool Valid() const;

        bool Create();
        bool Bind(uint16_t port);
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
        struct sockaddr_in servaddr_;
        static const int BACKLOG = 50;
    };

}

#endif //_KVSTORESERVER_SOCKET_HPP_
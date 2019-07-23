#ifndef _KVSTORESERVER_NET_SOCKET_HPP_
#define _KVSTORESERVER_NET_SOCKET_HPP_

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
        ~Socket();

        int Fd() const;
        void SetServerAddr(const sockaddr_in& addr);
        sockaddr_in GetServerAddr() const;
        bool Valid() const;

        bool Create();
        bool Bind(uint16_t port);
        bool Connect(const Address& addr);
        bool Listen();
        int Accept();
        bool Close();
        bool ShutdownWrite();
        bool ShutdownRead();

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

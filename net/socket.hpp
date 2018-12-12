#ifndef _NETWORK_SOCKET_HPP_
#define _NETWORK_SOCKET_HPP_

#include "address.hpp"

namespace Network{

class Socket
{
public:
    Socket();
    Socket(int fd);

    int Fd() const;
    bool Valid() const;

    bool Create();
    bool Bind(uint16_t port);
    bool Connet(const Address& addr);
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
    static const int BACKLOG = 50;
};

}

#endif //_NETWORK_SOCKET_HPP_
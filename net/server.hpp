#ifndef _NETWORK_SERVER_HPP_
#define _NETWORK_SERVER_HPP_

#include "socket.hpp"

namespace Network{

class Server
{
public:
    Server(uint16_t port);
    ~Server();

    void Start();
    void Close();
    int Accept();
    void Running();
    uint16_t Port() const;

private:
    void Echo(int client_sock);

    uint16_t port_;
    Socket socket_;
    int buffer_size;
};

}

#endif //_NETWORK_SERVER_HPP_
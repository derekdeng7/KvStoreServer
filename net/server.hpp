#ifndef _NETWORK_SERVER_HPP
#define _NETWORK_SERVER_HPP

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
    void Echo();
    uint16_t Port() const;
    //std::vector<std::shared_ptr<Connection>>& Connections();

private:
    uint16_t port_;
    Socket socket_;
    int buffer_size;
};

}

#endif //_NETWORK_SERVER_HPP
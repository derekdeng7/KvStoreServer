#include "server.hpp"
#include "epoll.hpp"

namespace Network{

    Server::Server(uint16_t port)
       :port_(port),
        pAcceptor_(NULL),
        loop_(new EventLoop())
    {}

    Server::~Server()
    {}

    void Server::Start()
    {
        pAcceptor_ = new Acceptor(loop_, port_); 
        pAcceptor_->SetConnectCallBack(this);
        pAcceptor_->Start();
        loop_->Loop();
    }

    void Server::NewConnection(int sockfd, sockaddr_in addr)
    {
        Connection* conn = new Connection(sockfd, addr, loop_);
        connections_[sockfd] = conn;
    }

}

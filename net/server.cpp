#include "server.hpp"
#include "epoll.hpp"

namespace KvStoreServer{

    Server::Server(uint16_t port)
       :port_(port),
        pAcceptor_(nullptr),
        loop_(nullptr)
    {}

    Server::~Server()
    {}

    void Server::Start()
    {
        loop_ = std::make_shared<EventLoop>();
        loop_->Start();

        pAcceptor_ = std::make_shared<Acceptor>(loop_, port_); 
        pAcceptor_->SetCallback(shared_from_this());
        pAcceptor_->Start();

        loop_->Loop();     
    }

    void Server::Stop()
    {

    }

    void Server::NewConnection(int sockfd, sockaddr_in addr)
    {
        auto conn = std::make_shared<Connector>(sockfd, addr, loop_);
        conn->Start();
        connections_[sockfd] = conn;
    }

}

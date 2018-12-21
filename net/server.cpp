#include <functional>

#include "server.hpp"
#include "epoll.hpp"

namespace KvStoreServer{

    Server::Server(uint16_t port)
       :port_(port),
        Acceptor_(nullptr),
        loop_(nullptr),
        threadPool_(nullptr)
    {}

    Server::~Server()
    {
        this->Close();
    }

    void Server::Start()
    {
        int numThread = std::thread::hardware_concurrency();
        threadPool_ = std::make_shared<ThreadPool>();
        threadPool_->Start(numThread - 2);
        
        loop_ = std::make_shared<EventLoop>();
        loop_->Start();

        Acceptor_ = std::make_shared<Acceptor>(loop_, port_); 
        Acceptor_->SetNewConnectionCallback(
            std::bind(&Server::NewConnection, shared_from_this(), std::placeholders::_1, std::placeholders::_2)
        );
        Acceptor_->Start();

        loop_->Loop();
    }

    void Server::Close()
    {
        threadPool_->Stop();
        ClearConnections();
        Acceptor_->Close();
        loop_->Close();
    }

    void Server::NewConnection(int sockfd, const sockaddr_in& addr)
    {
        auto connectChannel = std::make_shared<Connector>(sockfd, addr, loop_, threadPool_, shared_from_this());
        connectChannel->SetWriteCompleteCallback(
            std::bind(&Server::WriteComplete, shared_from_this())
        );
        connectChannel->Start();
        connections_[sockfd] = connectChannel;
    }

    void Server::WriteComplete()
    {
        std::cout << "[i] Writing Completed!" << std::endl;
    }

    void Server::CloseConnection(int sockfd)
    {
        auto iter = connections_.find(sockfd);
        if(iter != connections_.end())
        {
            (iter->second)->Close();
            (iter->second).reset();
            connections_.erase(sockfd);
            std::cout << "remove sockfd: " << sockfd << ", " << connections_.size() << " connection rest now"<< std::endl;
        }
    }

    void Server::ClearConnections()
    {
        auto iter = connections_.begin();
        while( iter != connections_.end())
        {
            (iter->second)->Close();
            (iter->second).reset();
            iter++;
        }
    }
}

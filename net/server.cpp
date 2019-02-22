#include <functional>

#include "server.hpp"
#include "epoll.hpp"

namespace KvStoreServer{

    Server::Server(uint16_t port)
       :port_(port),
        acceptor_(nullptr),
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

        acceptor_ = std::make_shared<Acceptor>(loop_, port_); 
        acceptor_->SetNewConnectionCallback(
            std::bind(&Server::NewConnection, this, std::placeholders::_1, std::placeholders::_2)
        );
        acceptor_->Start();

        loop_->Loop();
    }

    void Server::Close()
    {
         
        std::cout << "threadPool_.use_count: " << threadPool_.use_count() << std::endl;
        std::cout << "acceptor_.use_count: " << acceptor_.use_count() << std::endl;
        std::cout << "loop_.use_count: " << loop_.use_count() << std::endl;
        
        threadPool_->Stop();
        ClearConnections();
        acceptor_->Close();
        loop_->Close();
    }

    void Server::NewConnection(int sockfd, const sockaddr_in& addr)
    {
        auto connector = std::make_shared<Connector>(sockfd, addr, loop_, threadPool_, shared_from_this());
        connector->SetWriteCompleteCallback(
            std::bind(&Server::WriteComplete, this)
        );
        connector->Start();
        connections_[sockfd] = connector;
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

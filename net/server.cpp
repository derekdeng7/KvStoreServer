#include <functional>

#include "server.hpp"
#include "epoll.hpp"
#include "../db/lsmTree.hpp"

namespace KvStoreServer{

    Server::Server(size_t threadNum, uint16_t port)
      : threadNum_(threadNum),
        port_(port),
        loop_(nullptr),
        acceptor_(nullptr)
    {}

    Server::~Server()
    {
        this->Close();
    }

    void Server::Start()
    {
        LSMTree* lsmTree = LSMTree::getInstance();
        lsmTree->Start();

        loop_ = std::make_shared<EventLoop>(threadNum_);
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
        std::cout << "acceptor_.use_count: " << acceptor_.use_count() << std::endl;
        std::cout << "loop_.use_count: " << loop_.use_count() << std::endl;
        
        ClearConnections();
        acceptor_->Close();
        loop_->Close();
    }

    void Server::NewConnection(int sockfd, const sockaddr_in& addr)
    {
        auto connector = std::make_shared<Connector>(sockfd, addr, loop_);
        connector->SetRemoveConnectionCallback(
            std::bind(&Server::RemoveConnection, this, std::placeholders::_1)
        );
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

    void Server::RemoveConnection(int sockfd)
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

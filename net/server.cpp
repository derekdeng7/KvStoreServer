#include <functional>

#include "acceptor.hpp"
#include "connector.hpp"
#include "epoll.hpp"
#include "eventLoop.hpp"
#include "server.hpp"
#include "socket.hpp"
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
        //LSMTree* lsmTree = LSMTree::getInstance();
        //lsmTree->Start();

        loop_ = std::make_shared<EventLoop>(threadNum_);
        loop_->Start();

        acceptor_ = std::make_shared<Acceptor>(loop_, port_); 
        acceptor_->SetNewConnectionCallback(
            std::bind(&Server::NewConnection, this, std::placeholders::_1)
        );
        acceptor_->Start();

        loop_->Loop();
    }

    void Server::Close()
    {
        ClearConnections();
        acceptor_->Close();
        loop_->Close();
    }

    void Server::NewConnection(std::shared_ptr<Socket> socket)
    {
        auto connector = std::make_shared<Connector>(socket, loop_);
        connector->SetRecvCallback(
            std::bind(&Server::Receive, this, std::placeholders::_1, std::placeholders::_2)
            );
        connector->SetRemoveConnectionCallback(
            std::bind(&Server::RemoveConnection, this, std::placeholders::_1)
        );
        connector->SetWriteCompleteCallback(
            std::bind(&Server::WriteComplete, this)
        );
        connector->Start();
        connections_[socket->Fd()] = connector;
    }

    void Server::Receive(int sockfd, const std::string& message)
    {
        connections_[sockfd]->Send(message);
    }

    void Server::WriteComplete()
    {
        //std::cout << "[i] Writing Completed!" << std::endl;
    }

    void Server::RemoveConnection(int sockfd)
    {
        void (Server::*fp)(int sockfd) = &Server::RemoveConnectionInLoop;
        loop_->QueueInLoop(std::bind(fp, this, sockfd));
    }

    void Server::RemoveConnectionInLoop(int sockfd)
    {
        auto iter = connections_.find(sockfd);
        if(iter != connections_.end())
        {
            (iter->second)->Close();
            //(iter->second).reset();
            connections_.erase(sockfd);
            //std::cout << "remove sockfd: " << sockfd << ", " << connections_.size() << " connection rest now"<< std::endl;
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

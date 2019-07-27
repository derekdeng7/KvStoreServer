#include <functional>

#include "acceptor.hpp"
#include "connector.hpp"
#include "epoll.hpp"
#include "eventLoop.hpp"
#include "server.hpp"
#include "socket.hpp"
#include "timerQueue.hpp"
#include "timeStamp.hpp"

namespace KvStoreServer{

    Server::Server(uint16_t port)
      : port_(port),
        loop_(nullptr),
        acceptor_(nullptr)
    {}

    Server::~Server()
    {
        this->Close();
    }

    void Server::Start()
    {
        loop_ = std::make_shared<EventLoop>();
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

    void Server::Send(int sockfd, const std::string& message)
    {
       connections_[sockfd]->Send(message);
    }

    void Server::RunAt(TimeStamp time, TimerCallback cb)
    {
        if(loop_->IsInLoopThread())
        {
             loop_->RunAt(time, cb);
        }
        else
        {
            loop_->QueueInLoop(std::bind(&EventLoop::RunAt, loop_, time, cb));
        }
    }

    void Server::RunAfter(double delay, TimerCallback cb)
    {
        if(loop_->IsInLoopThread())
        {
             loop_->RunAfter(delay, cb);
        }
        else
        {
            loop_->QueueInLoop(std::bind(&EventLoop::RunAfter, loop_, delay, cb));
        }
    }

    void Server::RunEvery(double interval, TimerCallback cb)
    {
        if(loop_->IsInLoopThread())
        {
             loop_->RunEvery(interval, cb);
        }
        else
        {
            loop_->QueueInLoop(std::bind(&EventLoop::RunEvery, loop_, interval, cb));
        }
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
        //do services
        recvCallback_(sockfd, message);
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

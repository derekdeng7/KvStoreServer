#include "acceptor.hpp"
#include "connector.hpp"
#include "epoll.hpp"
#include "eventLoop.hpp"
#include "server.hpp"
#include "socket.hpp"
#include "timerQueue.hpp"
#include "timeStamp.hpp"

#include <functional>

namespace KvStoreServer{

    Server::Server(uint16_t port, int timeoutSecond )
      : port_(port),
        loop_(nullptr),
        acceptor_(nullptr),
        timeoutSecond_(timeoutSecond)
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

        if(timeoutSecond_)
        {
            connectionBuckets_.push(Bucket());
            loop_->RunEvery(1.0, std::bind(&Server::Timeout, this));
        }

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
       if(connections_.find(sockfd) != connections_.end())
            connections_[sockfd]->Send(message);
        else
        {
            std::cout << "connection fd: " << sockfd << " has been closed" << std::endl;
        }
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

        if(timeoutSecond_)
        {
            std::shared_ptr<HeartBeat> beat(new HeartBeat(connector));
            connectionBuckets_.back().insert(beat);

            std::weak_ptr<HeartBeat> weakBeat(beat);
            connector->SetHeartBeat(weakBeat);
        }
    }

    void Server::Receive(int sockfd, const std::string& message)
    {
        if(timeoutSecond_)
        {
            std::weak_ptr<HeartBeat> weakBeat(connections_[sockfd]->GetHeartBeat());
            std::shared_ptr<HeartBeat> beat(weakBeat.lock());
            if(beat)
            {
                connectionBuckets_.back().insert(beat);
            }
        }

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

    void Server::Timeout()
    {
        connectionBuckets_.push(Bucket());
        if(connectionBuckets_.size() > timeoutSecond_)
            connectionBuckets_.pop();
    }
}

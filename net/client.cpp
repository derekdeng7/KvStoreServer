#include "client.hpp"
#include "connector.hpp"
#include "eventLoop.hpp"
#include "socket.hpp"
#include "timerQueue.hpp"

namespace KvStoreServer{

    Client::Client()
      : loop_(new EventLoop())
    {}

    Client::~Client()
    {
        this->Close();
    }

     void Client::Start()
     {
        //loop_ = std::make_shared<EventLoop>();
        loop_->Start();

        loop_->Loop();
     }

    void Client::Close()
    {
         ClearConnections();
        loop_->Close();
    }

    int Client::Connect(const char* ip, uint16_t port)
    {
        std::shared_ptr<Socket> socket(new Socket());
        if(!socket->Create())
        {
            std::cout << "Create() failed, error code: " + std::to_string(errno) << std::endl;
            return -1;
        }

        if(!socket->Connect(Address(ip, port)))
        {
            std::cout << "Connect() failed, error code: " + std::to_string(errno) << std::endl;
            return -1;
        }

        if(!socket->SetNonBlock())
        {
            std::cout << "SetNonBlocking() failed, error code: " + std::to_string(errno) << std::endl;
            return -1;
        }

        NewConnection(socket);

        return socket->Fd();
    }

    void Client::Send(int sockfd, const std::string& message)
    {
        if(connections_.find(sockfd) != connections_.end())
            connections_[sockfd]->Send(message);
        else
        {
            std::cout << "connection fd: " << sockfd << " has been closed" << std::endl;
        }
    }

    void Client::NewConnection(std::shared_ptr<Socket> socket)
    {
        auto connector = std::make_shared<Connector>(socket, loop_);
        connector->SetRecvCallback(
            std::bind(&Client::Receive, this, std::placeholders::_1, std::placeholders::_2)
            );
        connector->SetRemoveConnectionCallback(
            std::bind(&Client::RemoveConnection, this, std::placeholders::_1)
            );
        connector->SetWriteCompleteCallback(
            std::bind(&Client::WriteComplete, this)
            );
        connector->Start();
        connections_[socket->Fd()] = connector;
    }

    void Client::Receive(int sockfd, const std::string& message)
    {
        recvCallback_(sockfd, message);
    }

    void Client::WriteComplete()
    {
       // std::cout << "[i] Writing Completed!" << std::endl;
    }

    void Client::RemoveConnection(int sockfd)
    {
        void (Client::*fp)(int sockfd) = &Client::RemoveConnectionInLoop;
        loop_->QueueInLoop(std::bind(fp, this, sockfd));
    }

    void Client::RemoveConnectionInLoop(int sockfd)
    {
        auto iter = connections_.find(sockfd);
        if(iter != connections_.end())
        {
            (iter->second)->Close();
            connections_.erase(sockfd);
            //std::cout << "remove sockfd: " << sockfd << ", " << connections_.size() << " connection rest now"<< std::endl;
        }
    }

    void Client::ClearConnections()
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

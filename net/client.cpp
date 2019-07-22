#include "client.hpp"

namespace KvStoreServer{

    Client::Client(const char* ip, uint16_t port, size_t threadNum)
      : serverAddr_(ip, port),
        loop_(nullptr),
        threadNum_(threadNum),
        sessions_(1000),
        finishSessions_(0),
        maxCounts_(16384),
        messageSize_(512)
    {}

    Client::~Client()
    {
        this->Close();
    }

    void Client::Start()
    {
        message_ = std::string(messageSize_, 'a');

        loop_ = std::make_shared<EventLoop>(threadNum_);
        loop_->Start();
        
        for(size_t i = 0; i < sessions_; i++)
        {
            std::shared_ptr<Socket> socket(new Socket());
            if(Connect(socket))
            {
              NewConnection(socket);
              counts_[socket->Fd()] = 0;
            }
        }

        timer_.reset();

         for(auto iter : connections_)
        {
            Send(iter.first, message_);
        }

        loop_->Loop();
    }

    void Client::Close()
    {
         ClearConnections();
        loop_->Close();
    }

    bool Client::Connect(std::shared_ptr<Socket> socket)
    {
        if(!socket->Create())
        {
            std::cout << "Create() failed, error code: " + std::to_string(errno) << std::endl;
            return false;
        }

        if(!socket->Connect(serverAddr_))
        {
            std::cout << "Connect() failed, error code: " + std::to_string(errno) << std::endl;
            return false;
        }

        if(!socket->SetNonBlock())
        {
            std::cout << "SetNonBlocking() failed, error code: " + std::to_string(errno) << std::endl;
            return false;
        }

        return true;
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

        //connector->Send(message_);
    }

    void Client::Receive(int sockfd, std::string& message)
    {
        if(++counts_[sockfd] >= maxCounts_)
        {
            //std::cout << "sockfd: " << sockfd << " finishes tasks and shutdowns" << std::endl;
            shutdown(sockfd, SHUT_WR);

            if(++finishSessions_ == sessions_)
            {
                double time =  static_cast<double>(timer_.elapsed_milli()) / 1000;
                std::cout << "///////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;
               
                std::cout << "[i] totally " << static_cast<size_t>(sessions_ * messageSize_ * maxCounts_ / 1048576) << " MB sent in " << time << " seconds,  " 
                << static_cast<int>(sessions_ * maxCounts_  /  time) << " QPS"<< std::endl << std::endl;
                
                std::cout << "///////////////////////////////////////////////////////////////////////////////////" <<std::endl;

                loop_->Close();
            }
        }
        else
        {
            Send(sockfd, message);
        }
    }

    void Client::WriteComplete()
    {
       // std::cout << "[i] Writing Completed!" << std::endl;
    }

    void Client::RemoveConnection(int sockfd)
    {
        void (Client::*fp)(int sockfd) = &Client::RemoveConnectionInLoop;
        loop_->queueInLoop(std::bind(fp, this, sockfd));
    }

    void Client::RemoveConnectionInLoop(int sockfd)
    {
        auto iter = connections_.find(sockfd);
        if(iter != connections_.end())
        {
            (iter->second)->Close();
            //(iter->second).reset();
            connections_.erase(sockfd);
            counts_.erase(sockfd);
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

    void Client::Send(int sockfd, const std::string& message)
    {
       connections_[sockfd]->Send(message);
    }

}

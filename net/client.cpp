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
            Socket socket;
            if(Connect(socket))
            {
              NewConnection(socket.Fd(), socket.ServerAddr());
              counts_[socket.Fd()] = 0;
            }
        }

        timer_.reset();

        loop_->Loop();
    }

    void Client::Close()
    {
         ClearConnections();
        loop_->Close();
    }

    bool Client::Connect(Socket& socket)
    {
        if(!socket.Create())
        {
          std::cout << "Create() failed, error code: " + std::to_string(errno) << std::endl;
          return false;
        }

        if(!socket.Connect(serverAddr_))
        {
          std::cout << "Connect() failed, error code: " + std::to_string(errno) << std::endl;
          return false;
        }

        if(!socket.SetNonBlock())
        {
          std::cout << "SetNonBlocking() failed, error code: " + std::to_string(errno) << std::endl;
          return false;
        }

        return true;
    }

    void Client::NewConnection(int sockfd, const sockaddr_in& addr)
    {
        auto connector = std::make_shared<Connector>(sockfd, addr, loop_);
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
        connections_[sockfd] = connector;

        connector->Send(message_);
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
        auto iter = connections_.find(sockfd);
        if(iter != connections_.end())
        {
            (iter->second)->Close();
            (iter->second).reset();
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

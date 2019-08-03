#include "benchClient.hpp"
#include "../net/address.hpp"
#include "../net/connector.hpp"
#include "../net/eventLoop.hpp"
#include "../net/socket.hpp"
#include "../net/timeStamp.hpp"

namespace KvStoreServer{

    BenchClient::BenchClient(const char* ip, uint16_t port, size_t sessions, size_t messageNum, size_t messageSize)
      : ip_(ip),
        port_(port),
        loop_(nullptr),
        sessions_(sessions),
        finishSessions_(0),
        messageNum_(messageNum),
        messageSize_(messageSize)
    {}

    BenchClient::~BenchClient()
    {
        this->Close();
    }

     void BenchClient::StartBench()
     {
        message_ = std::string(messageSize_, 'a');

        loop_ = std::make_shared<EventLoop>();
        loop_->Start();
        
        for(size_t i = 0; i < sessions_; i++)
        {
            int fd = Connect(ip_, port_);
            if(fd != -1)
            {
                counts_[fd] = 0;
            }
        }

        stamp_->Init();

        for(auto iter : counts_)
        {
            Send(iter.first, message_);
        }

        loop_->Loop();
     }

    void BenchClient::Close()
    {
         ClearConnections();
        loop_->Close();
    }

    int BenchClient::Connect(const char* ip, uint16_t port)
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

    void BenchClient::Send(int sockfd, const std::string& message)
    {
       connections_[sockfd]->Send(message);
    }

    void BenchClient::NewConnection(std::shared_ptr<Socket> socket)
    {
        auto connector = std::make_shared<Connector>(socket, loop_);
        connector->SetRecvCallback(
            std::bind(&BenchClient::Receive, this, std::placeholders::_1, std::placeholders::_2)
            );
        connector->SetRemoveConnectionCallback(
            std::bind(&BenchClient::RemoveConnection, this, std::placeholders::_1)
            );
        connector->SetWriteCompleteCallback(
            std::bind(&BenchClient::WriteComplete, this)
            );
        connector->Start();
        connections_[socket->Fd()] = connector;
    }

    void BenchClient::Receive(int sockfd, const std::string& message)
    {
        if(++counts_[sockfd] >= messageNum_)
        {
            //std::cout << "sockfd: " << sockfd << " finishes tasks and shutdowns" << std::endl;
            shutdown(sockfd, SHUT_WR);

            if(++finishSessions_ == sessions_)
            {
                double time =  static_cast<double>(stamp_->MicroSecondsSinceInit() / 1000 / 1000);
                std::cout << "///////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;        
            
                std::cout << "[i] totally " << static_cast<size_t>(sessions_ * messageSize_ * messageNum_ / 1048576) << " MB sent in " << time << " seconds,  " 
                    << static_cast<int>(sessions_ * messageNum_  /  time) << " QPS"<< std::endl << std::endl;
                    
                std::cout << "///////////////////////////////////////////////////////////////////////////////////" <<std::endl;

                loop_->Close();
            }
        }
        else
        {
            Send(sockfd, message);
        }
    }

    void BenchClient::WriteComplete()
    {
       // std::cout << "[i] Writing Completed!" << std::endl;
    }

    void BenchClient::RemoveConnection(int sockfd)
    {
        void (BenchClient::*fp)(int sockfd) = &BenchClient::RemoveConnectionInLoop;
        loop_->QueueInLoop(std::bind(fp, this, sockfd));
    }

    void BenchClient::RemoveConnectionInLoop(int sockfd)
    {
        auto iter = connections_.find(sockfd);
        if(iter != connections_.end())
        {
            (iter->second)->Close();
            connections_.erase(sockfd);
            //std::cout << "remove sockfd: " << sockfd << ", " << connections_.size() << " connection rest now"<< std::endl;
        }
    }

    void BenchClient::ClearConnections()
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

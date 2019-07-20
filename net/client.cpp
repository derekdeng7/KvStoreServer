#include "client.hpp"

namespace KvStoreServer{

    Client::Client(const char* ip, uint16_t port, int nums)
      : serverAddr_(ip, port),
        loop_(nullptr),
        fdNums_(nums)
    {}

    Client::~Client()
    {
        this->Close();
    }

    void Client::Start()
    {
        message_ = std::string(1024, 31);

        loop_ = std::make_shared<EventLoop>(1);
        loop_->Start();
        
        for(int i = 0; i < fdNums_; i++)
        {
            Socket socket;
            if(Connect(socket))
            {
              NewConnection(socket.Fd(), socket.ServerAddr());
              counts_[socket.Fd()] = 0;
            }
        }

        loop_->Loop();
    }

    void Client::Close()
    {
        std::cout << "loop_.use_count: " << loop_.use_count() << std::endl;

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

        connector->Send("hello");
    }

    void Client::Receive(int sockfd, std::string& message)
    {
        if(++counts_[sockfd] >= 1000)
        {
            RemoveConnection(sockfd);
        }
        else
        {
          message = message_;
        }
    }

    void Client::WriteComplete()
    {
        std::cout << "[i] Writing Completed!" << std::endl;
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
            std::cout << "remove sockfd: " << sockfd << ", " << connections_.size() << " connection rest now"<< std::endl;
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

    void Client::Send(const std::string& message)
    {
        //connector_->Send(message);
    }

}

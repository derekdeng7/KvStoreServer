#include "server.hpp"
#include "epoll.hpp"

namespace KvStoreServer{

    Server::Server(uint16_t port)
       :port_(port),
        Acceptor_(nullptr),
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

        Acceptor_ = std::make_shared<Acceptor>(loop_, port_); 
        Acceptor_->SetCallback(shared_from_this());
        Acceptor_->Start();

        loop_->Loop();
    }

    void Server::Close()
    {
        threadPool_->Stop();
        ClearConnection();
        Acceptor_->Close();
        loop_->Close();
    }

    void Server::NewConnection(int sockfd, sockaddr_in addr)
    {
        auto conn = std::make_shared<Connector>(sockfd, addr, loop_, threadPool_);
        conn->Start();
        connections_[sockfd] = conn;
    }

    void Server::ClearConnection()
    {
        auto iter = connections_.begin();
        while( iter != connections_.end())
        {
            (iter->second)->Close();
            (iter->second).reset();
            iter++;
        }

        connections_.clear();
    }
}

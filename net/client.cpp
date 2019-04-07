#include "client.hpp"

namespace KvStoreServer{

    Client::Client(const char* ip, uint16_t port)
      : socket_(new Socket(-1)),
        serverAddr_(ip, port),
        loop_(nullptr),
        connector_(nullptr)
    {}

    Client::~Client()
    {
        this->Close();
    }

    void Client::Start()
    {
        loop_ = std::make_shared<EventLoop>(1);
        loop_->Start();
        
        connector_ = std::make_shared<Connector>(serverAddr_, loop_);
        connector_->SetRemoveConnectionCallback(
            std::bind(&Client::RemoveConnection, this, std::placeholders::_1)
        );
        connector_->SetWriteCompleteCallback(
            std::bind(&Client::WriteComplete, this)
        );
        connector_->Start();

        loop_->Loop();
    }

    void Client::Close()
    {
        std::cout << "loop_.use_count: " << loop_.use_count() << std::endl;

        loop_->Close();
    }

    void Client::Send(const std::string& message)
    {
        connector_->Send(message);
    }

    void Client::WriteComplete()
    {
        std::cout << "[i] Writing Completed!" << std::endl;
    }

    void Client::RemoveConnection(int sockfd)
    {
        std::cout << "close sockfd: " << sockfd << std::endl; 
        connector_->Close();
    }
}

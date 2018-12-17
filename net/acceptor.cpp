#include "acceptor.hpp"

namespace KvStoreServer{

    Acceptor::Acceptor(std::shared_ptr<EventLoop> loop, uint16_t port)
       :socket_(std::make_shared<Socket>(-1)),
        port_(port),
        listenfd_(-1),
        pAcceptChannel_(nullptr),
        pServerCallback_(nullptr),
        loop_(loop)
    {}

    Acceptor::~Acceptor()
    {}

    void Acceptor::Start()
    {
        listenfd_ = InitListenfd();

        pAcceptChannel_ = std::make_shared<Channel>(listenfd_, socket_->Serveraddr(), loop_);
        pAcceptChannel_->SetCallback(shared_from_this());
        pAcceptChannel_->EnableReading();
    }
    
    void Acceptor::SetCallback(std::shared_ptr<Server> pServerCallback)
    {
        pServerCallback_ = pServerCallback;
    }

    void Acceptor::HandleReading()
    {
        int connfd;
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(struct sockaddr_in);
        connfd = accept(listenfd_, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
        if(connfd > 0)
        {
            std::cout << "[+] new connection from " << inet_ntoa(cliaddr.sin_addr) 
                << ":" << ntohs(cliaddr.sin_port) << " new socket fd: " << connfd << std::endl;
        }
        else
        {
            std::string err = "accept error, connfd: " + std::to_string(connfd);
            perror(err.c_str());
        }

        if(!SetNonBlocking(connfd))
        {
            perror("fail to set non-blocking io");
            return;
        }

        pServerCallback_->NewConnection(connfd, cliaddr);
    }

    void Acceptor::HandleWriting()
    {}

    int Acceptor::InitListenfd()
    {
        if(!socket_->Create())
            throw std::runtime_error("Create() failed, error code: " + std::to_string(errno));

        if(!socket_->SetReuseAddress())
            throw std::runtime_error("SetReuseAddress() failed, error code: " + std::to_string(errno));
        
        if(!socket_->SetNonBlock())
            throw std::runtime_error("SetNonBlocking() failed, error code: " + std::to_string(errno));
        
        if(!socket_->Bind(port_))
            throw std::runtime_error("Bind() failed, error code: " + std::to_string(errno));
        
        if(!socket_->Listen())
            throw std::runtime_error("Listen() failed, error code: " + std::to_string(errno));
        
        std::cout << "Acceptor starts to listen, waiting for connection..." << std::endl;

        return socket_->Fd();
    }

    bool Acceptor::SetNonBlocking(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);
		if(flags == -1)
			return false;

		flags |= O_NONBLOCK;

		if(fcntl(fd, F_SETFL, flags) == -1)
			return false;

		return true;
    }
}
#include "acceptor.hpp"

namespace KvStoreServer{

    Acceptor::Acceptor(std::shared_ptr<EventLoop> loop, uint16_t port)
       :socket_(new Socket(-1)),
        port_(port),
        listenfd_(-1),
        acceptChannel_(nullptr),
        loop_(loop)
    {}

    Acceptor::~Acceptor()
    {
        this->Close();
    }

    void Acceptor::Start()
    {
        listenfd_ = InitListenfd();

        acceptChannel_.reset(new Channel(listenfd_, loop_));
        acceptChannel_->SetReadCallback(
            std::bind(&Acceptor::HandleRead, this)
        );
        acceptChannel_->AddChannel();
    }

    void Acceptor::Close()
    {
        acceptChannel_->RemoveChannel();
    }
    
    void Acceptor::SetNewConnectionCallback(const NewConnectionCallback& callback)
    {
        newConnectionCallback_ = callback;
    }

    void Acceptor::HandleRead()
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

        if(newConnectionCallback_)
        {
            std::shared_ptr<Socket> socket(new Socket(connfd));
            newConnectionCallback_(socket);
        }
        else
        {
            close(connfd);
        }
            
    }

    int Acceptor::InitListenfd()
    {
        if(!socket_->Create())
        {
            throw std::runtime_error("Create() failed, error code: " + std::to_string(errno));
        }

        if(!socket_->SetReuseAddress())
        {
            throw std::runtime_error("SetReuseAddress() failed, error code: " + std::to_string(errno));
        }

        if(!socket_->SetNonBlock())
        {
            throw std::runtime_error("SetNonBlocking() failed, error code: " + std::to_string(errno));
        }

        if(!socket_->Bind(port_))
        {
            throw std::runtime_error("Bind() failed, error code: " + std::to_string(errno));
        }

        if(!socket_->Listen())
        {
            throw std::runtime_error("Listen() failed, error code: " + std::to_string(errno));
        }

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

#include "connector.hpp"
#include "threadPool.hpp"

namespace KvStoreServer{

    Connector::Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop, std::shared_ptr<ThreadPool> threadPool, std::shared_ptr<Server> server)
       :sockfd_(sockfd),
        addr_(addr),
        channel_(nullptr),
        loop_(loop),
        recvBuf_(new Buffer()),
        sendBuf_(new Buffer()),
        threadPool_(threadPool),
        server_(server)     
    {}

    Connector::~Connector()
    {
        this->Close();
        std::cout << "Connector desctruct" << std::endl;
    }

    void Connector::Start()
    {
        channel_ = std::make_shared<Channel>(sockfd_, addr_, loop_);
        channel_->SetReadCallback(
            std::bind(&Connector::HandleRead, this)
        );
        channel_->SetWriteCallback(
            std::bind(&Connector::HandleWrite, this)
        );
        channel_->AddChannel();
    }

    void Connector::Close()
    {
        channel_->RemoveChannel();
    }

    void Connector::Send(const std::string& message)
    {
        if(loop_->isInLoopThread())
        {
            SendInLoop(message);
        }
        else
        {
            TaskInEventLoop task(std::bind(&Connector::SendInLoop, shared_from_this(), std::placeholders::_1), message);
            loop_->runInLoop(task);
        }
    }

    void Connector::SendInLoop(const std::string& message)
    {
        int n = 0;
        if(sendBuf_->DataSize() == 0)
        {
            std::cout << "[i] send: '" << message << "'" << std::endl;
            n = write(sockfd_, message.c_str(), message.size());
            if(n < 0)
            {
                std::cout << "[!] Connector::Send() write error" << std::endl;
            }

            if(n == static_cast<int>(message.size()))
            {
                writeCompleteCallback_();
            }
                
        }

        if(n < static_cast<int>(message.size()))
        {
            sendBuf_->Append(message.substr(n, message.size()));
            if(channel_->IsWriting())
            {
                channel_->EnableWriting();
            }
        }
    }

    void Connector::HandleRead()
    {
        int sockfd = channel_->GetSockfd();
        int read_size;
        char buf[BUF_SIZE];
        if(sockfd < 0)
        {
            perror("EPOLLIN sockfd < 0 error ");
            return;
        }
        bzero(buf, BUF_SIZE);
        if((read_size = read(sockfd, buf, BUF_SIZE)) < 0)
        {
            if(errno == ECONNRESET)
            {
                std::string err = "ECONNREST closed socket fd:" + std::to_string(sockfd);
                perror(err.c_str());
                close(sockfd);
            }
        }
        else if(read_size == 0)
        {
            std::cout << "[-] read 0, closed socket " << inet_ntoa(addr_.sin_addr) << ":" << ntohs(addr_.sin_port) << std::endl; 
            TaskInEventLoop task(std::bind(&Server::CloseConnection, server_, std::placeholders::_1), sockfd);
            loop_->queueInLoop(task);
        }
        else
        {
            std::string message;
            std::string strbuf(buf);
            recvBuf_->Append(strbuf);
            std::cout << "[i] receive from " << inet_ntoa(addr_.sin_addr) << ":" << ntohs(addr_.sin_port) << " : " << recvBuf_->GetChar() << std::endl; 
            
            TaskInSyncQueue task(std::bind(&Connector::Send, shared_from_this(), std::placeholders::_1), recvBuf_->RetriveAllAsString());
            threadPool_->AddTask(task);
        }
    }

    void Connector::HandleWrite()
    {
        int sockfd = channel_->GetSockfd();
        if(channel_->IsWriting())
        {
            int n = write(sockfd, sendBuf_->GetChar(), sendBuf_->DataSize());
            if(n > 0)
            {
                std::cout << "write " << n << " bytes data again" << std::endl;
                sendBuf_->Retrieve(n); 
                if(sendBuf_->DataSize() == 0)
                {
                    channel_->DisableWriting();

                }
            }
        }
    }

    void Connector::SetWriteCompleteCallback(EventCallback callback)
    {
        writeCompleteCallback_ = callback;
    }

}

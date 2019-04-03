#include "connector.hpp"
#include "server.hpp"

namespace KvStoreServer{

    Connector::Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop, std::shared_ptr<ThreadPool<TaskInSyncQueue>> threadPool, std::shared_ptr<Server> server)
       :sockfd_(sockfd),
        addr_(addr),
        channel_(nullptr),
        recvBuf_(new Buffer()),
        sendBuf_(new Buffer()),
        loop_(loop),
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
        channel_.reset(new Channel(sockfd_, addr_, loop_));
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

    void Connector::Send(const Message& message)
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

    void Connector::SendInLoop(const Message& message)
    {
        size_t n = 0;
        if(sendBuf_->DataSize() == 0)
        {
            std::cout << "[i] send Message " << std::endl;
            n = write(sockfd_, (char*)&message, sizeof(Message));
            if(n < 0)
            {
                std::cout << "[!] Connector::Send() write error" << std::endl;
            }

            if(n == sizeof(message))
            {
                writeCompleteCallback_();
            }
                
        }

        if(n < sizeof(Message))
        {
            assert(false);
            //sendBuf_->Append(message.substr(n, message.size()));
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

        Message msg;

        if(sockfd < 0)
        {
            perror("EPOLLIN sockfd < 0 error ");
            return;
        }
        bzero(buf, BUF_SIZE);
        //if((read_size = read(sockfd, buf, BUF_SIZE)) < 0)
        if((read_size = read(sockfd, (char*)&msg, sizeof(msg))) < 0)
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
            TaskInSyncQueue task(std::bind(&Connector::Send, shared_from_this(), std::placeholders::_1), msg);
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

#include "connector.hpp"
#include "threadPool.hpp"

namespace KvStoreServer{

    Connector::Connector(int sockfd, sockaddr_in addr, std::shared_ptr<EventLoop> loop)
       :sockfd_(sockfd),
        addr_(addr),
        pChannel_(nullptr),
        loop_(loop),
        recvBuf_(std::make_shared<Buffer>()),
        sendBuf_(std::make_shared<Buffer>())     
    {}

    Connector::~Connector()
    {}

    void Connector::Start()
    {
        pChannel_ = std::make_shared<Channel>(sockfd_, addr_, loop_);
        pChannel_->SetCallback(shared_from_this());
        pChannel_->EnableReading();
    }

    void Connector::Send(const std::string& message)
    {
        //std::cout << "Connector::Send" << std::endl;
        if(loop_->isInLoopThread())
        {
            SendInLoop(message);
        }
        else
        {
            TaskInEventLoop task(shared_from_this(), message);
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
                std::cout << "Connector::Send() write error" << std::endl;
            }

            if(n == static_cast<int>(message.size()))
            {
                TaskInEventLoop task(shared_from_this());
                loop_->queueInLoop(task);
            }
                
        }

        if(n < static_cast<int>(message.size()))
        {
            sendBuf_->Append(message.substr(n, message.size()));
            if(pChannel_->IsWriting())
            {
                pChannel_->EnableWriting();
            }
        }
    }

    void Connector::WriteComplete()
    {
        std::cout << "Writing Completed!" << std::endl;
    }

    void Connector::HandleReading()
    {
        //std::cout << "Connector::HandleReading" << std::endl;
        int sockfd = pChannel_->GetSockfd();
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
            std::cout << "[i] read 0, closed socket " << inet_ntoa(addr_.sin_addr) << ":" << ntohs(addr_.sin_port) << std::endl; 
            close(sockfd);
        }
        else
        {
            std::string message;
            std::string strbuf(buf);
            recvBuf_->Append(strbuf);
            std::cout << "[i] receive from " << inet_ntoa(addr_.sin_addr) << ":" << ntohs(addr_.sin_port) << " : " << recvBuf_->GetChar() << std::endl; 
            
            TaskInSyncQueue task(shared_from_this(), recvBuf_->RetriveAllAsString());
            ThreadPool* thdPool = ThreadPool::getInstance();
            thdPool->AddTask(task);
            
            /*
            if(write(sockfd, buf, read_size) != read_size)
            {
                perror("error: not finished in one time");
            }
            */
        }
    }

    void Connector::HandleWriting()
    {
        std::cout << "Connector::HandleWriting" << std::endl;
        int sockfd = pChannel_->GetSockfd();
        if(pChannel_->IsWriting())
        {
            int n = write(sockfd, sendBuf_->GetChar(), sendBuf_->DataSize());
            if(n > 0)
            {
                std::cout << "write " << n << " bytes data again" << std::endl;
                sendBuf_->Retrieve(n); 
                if(sendBuf_->DataSize() == 0)
                {
                    pChannel_->DisableWriting();

                }
            }
        }
    }

}
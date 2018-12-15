#include "connector.hpp"

namespace Network{

    Connector::Connector(int sockfd, sockaddr_in addr, EventLoop* loop)
       :sockfd_(sockfd),
        addr_(addr),
        pChannel_(NULL),
        loop_(loop),
        recvBuf_(new Buffer()),
        sendBuf_(new Buffer())     
    {
        pChannel_ = new Channel(sockfd_, addr_, loop_);
        pChannel_->SetCallBack(this);
        pChannel_->EnableReading();
    }

    Connector::~Connector()
    {}

    void Connector::Send(std::string message)
    {
        //std::cout << "Connector::Send" << std::endl;
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
                loop_->queueLoop(this);
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
            std::string strbuf(buf);
            recvBuf_->Append(strbuf);
            std::cout << "[i] receive from " << inet_ntoa(addr_.sin_addr) << ":" << ntohs(addr_.sin_port) << " : " << recvBuf_ << std::endl; 
            
            while(recvBuf_->DataSize() > 0)
            {
                if(recvBuf_->DataSize() >= MESSAGE_SIZE)
                {
                    std::string message = recvBuf_->RetriveAsString(MESSAGE_SIZE);
                    this->Send(message);
                }
                else
                {
                    std::string message = recvBuf_->RetriveAllAsString();
                    this->Send(message);
                }
                
            }
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
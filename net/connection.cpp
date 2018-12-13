#include "connection.hpp"

namespace Network{

    Connection::Connection(int sockfd, sockaddr_in addr, EventLoop* loop)
       :sockfd_(sockfd),
        addr_(addr),
        pChannel_(NULL),
        loop_(loop)      
    {
        pChannel_ = new Channel(sockfd_, addr_, loop_);
        pChannel_->SetConnectCallBack(this);
        pChannel_->EnableReading();
    }

    Connection::~Connection()
    {}

    void Connection::HandleCallBack(int sockfd)
    {
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
            std::cout << "[i] receive from " << inet_ntoa(addr_.sin_addr) << ":" << ntohs(addr_.sin_port) << " : " << buf << std::endl; 
            if(write(sockfd, buf, read_size) != read_size)
            {
                perror("error: not finished in one time");
            }
        }
    }
}
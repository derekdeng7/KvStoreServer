#include "channel.hpp"
#include "connector.hpp"
#include "epoll.hpp"
#include "server.hpp"
#include "socket.hpp"

namespace KvStoreServer{

    Connector::Connector(std::shared_ptr<Socket> socket, std::shared_ptr<EventLoop> loop)
       :socket_(socket),
        connChannel_(nullptr),
        recvBuf_(new Buffer()),
        sendBuf_(new Buffer()),
        loop_(loop),
        isMultiThread_(loop_->GetThreadNum() > 0)
    {}

    Connector::~Connector()
    {
        this->Close();
        //std::cout << "Connector desctruct" << std::endl;
    }


    void Connector::Start()
    {
        connChannel_.reset(new Channel(socket_->Fd(), loop_));
        connChannel_->SetReadCallback(
            std::bind(&Connector::HandleRead, this)
        );
        connChannel_->SetWriteCallback(
            std::bind(&Connector::HandleWrite, this)
        );
        connChannel_->AddChannel();
    }

    void Connector::Close()
    {
        connChannel_->RemoveChannel();
    }

    void Connector::Send(const std::string& message)
    {
        if(loop_->IsInLoopThread())
        {
            SendInLoop(message);
        }
        else
        {
            void (Connector::*fp)(const std::string& message) = &Connector::SendInLoop;
            loop_->RunInLoop(std::bind(fp, shared_from_this(), message));
        }
    }

    void Connector::SendInLoop(const std::string& message)
    {
        int n = 0;
        if(sendBuf_->DataSize() == 0)
        {
            //std::cout << "[i] send: '" << message << "'" << std::endl;
            n = write(socket_->Fd(), message.c_str(), message.size());
            if(n < 0)
            {
                std::cout << "[!] Connector::Send() write error" << std::endl;
                connChannel_->EnableWriting();
                return;
            }

            if(n == static_cast<int>(message.size()))
            {
                writeCompleteCallback_();
            }
        }

        if(n < static_cast<int>(message.size()))
        {
            sendBuf_->Append(message.substr(n, message.size()));
            {
                connChannel_->EnableWriting();
            }
        }
    }

    void Connector::Shutdown()
    {
        loop_->RunInLoop(std::bind(&Connector::ShutdownInLoop, shared_from_this()));
    }

    void Connector::ShutdownInLoop()
    {
        socket_->ShutdownWrite();
    }
    
    void Connector::ForceClose()
    {
        loop_->QueueInLoop(std::bind(&Connector::ForceCloseInLoop, shared_from_this()));
    }

    void Connector::ForceCloseInLoop()
    {
        HandleClose();
    }

    void Connector::HandleRead()
    {
        int sockfd = connChannel_->GetFd();
        int read_size;
        char buf[BUFSIZE];

        if(sockfd < 0)
        {
            perror("EPOLLIN sockfd < 0 error ");
            return;
        }

        bzero(buf, BUFSIZE);
        if((read_size = read(sockfd, buf, BUFSIZE)) < 0)
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
            //std::cout << "[-] read 0, closed socket " << inet_ntoa(socket_->ServerAddr().sin_addr) << ":" << ntohs(socket_->ServerAddr().sin_port) << std::endl; 
           HandleClose();
        }
        else
        {
            std::string strbuf(buf);
            recvBuf_->Append(strbuf);
            //std::cout << "[i] receive from " << inet_ntoa(socket_->ServerAddr().sin_addr) << ":" << ntohs(socket_->ServerAddr().sin_port) << " : " << recvBuf_->GetChar() << std::endl; 
            
            std::string message = recvBuf_->RetriveAllAsString();
            recvCallback_(socket_->Fd(), message);
            /*
            if(isMultiThread_)
            {
                TaskInSyncQueue task(std::bind(&Connector::Send, this, std::placeholders::_1), message);
                loop_->AddTask(task);
            }
            */
        }
    }

    void Connector::HandleWrite()
    {
        int sockfd = connChannel_->GetFd();
        if(connChannel_->IsWriting())
        {
            int n = write(sockfd, sendBuf_->GetChar(), sendBuf_->DataSize());
            if(n > 0)
            {
                //std::cout << "write " << n << " bytes data again" << std::endl;
                sendBuf_->Retrieve(n); 
                if(sendBuf_->DataSize() == 0)
                {
                    connChannel_->DisableWriting();
                }
            }
        }
    }

    void Connector::HandleClose()
    {
        removeConnectionCallback_(connChannel_->GetFd());
    }

}

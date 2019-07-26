#include "kvClient.hpp"
#include "../net/timerQueue.hpp"

#include <functional>

namespace KvStoreServer{

    KvClient::KvClient()
      : client_(nullptr),
        thread_(nullptr)
    {}

    KvClient::~KvClient()
    {
        if(thread_)
        {
            thread_->join();
        }
    }

    void KvClient::Start()
    {
        client_ = std::make_shared<Client>();
        client_->SetRecvCallback(
            std::bind(&KvClient::Receive, this, std::placeholders::_1, std::placeholders::_2)
            );

        thread_ = std::make_shared<std::thread>(&Client::Start, client_);
    }

    int KvClient::CreateConnection(const char* serverIp, uint16_t port)
    {
        return client_->Connect(serverIp, port);
    }

    void KvClient::Receive(int sockfd, const std::string& message)
    {
        std::cout << "sockfd: " << sockfd << " receive from server: " << message << std::endl;
        /* 
        if(++counts_[sockfd] >= messageNum_)
        {
            //std::cout << "sockfd: " << sockfd << " finishes tasks and shutdowns" << std::endl;
            shutdown(sockfd, SHUT_WR);
            sessions_--;
        }
        else
        {
            Send(sockfd, message);
        }
        */
    }

    void KvClient::Send(int sockfd, const std::string& message)
    {
        client_->Send(sockfd, message);
    }

    void KvClient::StartBench(const char* serverIp, uint16_t port, size_t sessions, size_t messageNum, size_t messageSize)
    {
        sessions_ = sessions;
        messageNum_ = messageNum;
        message_ = std::string(messageSize, 'a');
        
        for(size_t i = 0; i < sessions_; i++)
        {
            int fd = CreateConnection(serverIp, port);
            if(fd != -1)
            {
                counts_[fd] = 0;
            }
        }

        stamp_.Init();

        for(auto iter : counts_)
        {
            Send(iter.first, message_);
        }

        while(sessions_);

        double time =  static_cast<double>(stamp_.MicroSecondsSinceInit() / 1000 / 1000);
        std::cout << "///////////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;        
            
        std::cout << "[i] totally " << static_cast<size_t>(sessions * messageSize * messageNum_ / 1048576) << " MB sent in " << time << " seconds,  " 
            << static_cast<int>(sessions * messageNum_  /  time) << " QPS"<< std::endl << std::endl;
                    
        std::cout << "///////////////////////////////////////////////////////////////////////////////////" <<std::endl;

        Loop();
    }

    void KvClient::Loop()
    {
        //ShowHelp();

        while(true)
        {
            std::string command;
            getline(std::cin, command);

            argVec_.clear();
            std::string arg;
            std::stringstream ss(command);
            while(ss >> arg)
            {
                argVec_.push_back(arg);
            }

            if(!strcasecmp(argVec_[0].c_str(), "quit"))
            {
                exit(0);
            }
            else if(!strcasecmp(argVec_[0].c_str(), "help"))
            {
                ShowHelp();
                continue;
            }

            if(CheckCommand())
            {
                //client_->Send(command);
            }
        }
    }

    void KvClient::ShowHelp()
    {
        std::cout << "   -----------Help Document-----------" << std::endl;
        std::cout << "   commands below are case insensitive" << std::endl << std::endl;
        std::cout << "   put key value" << std::endl;
        std::cout << "   get key" << std::endl;
        std::cout << "   update key value" << std::endl;
        std::cout << "   remove key" << std::endl;
        std::cout << "   quit" << std::endl;
        std::cout << "   help" << std::endl;
        std::cout << "   ----------------END----------------" << std::endl << std::endl;
    }

    bool KvClient::CheckCommand()
    {
        int argNum = argVec_.size();
        
        if(!strcasecmp(argVec_[0].c_str(), "put") && argNum == 3)
        {
            return true;
        }
        else if(!strcasecmp(argVec_[0].c_str(), "get") && argNum == 2)
        {
            return true;
        }
        else if(!strcasecmp(argVec_[0].c_str(), "update") && argNum == 3)
        {
            return true;
        }
        else if(!strcasecmp(argVec_[0].c_str(), "remove") && argNum == 2)
        {
            return true;
        }

        std::cout << "Argument Error. Use \"help\" for help." << std::endl;
        return false;
    }

} 

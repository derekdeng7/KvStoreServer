#include "kvClient.hpp"

namespace KvStoreServer{

    KvClient::KvClient(const char* serverIp, uint16_t port)
      : serverIP_(serverIp), 
        port_(port),
        client_(nullptr),
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
        client_ = std::make_shared<Client>(serverIP_, port_);
        thread_ = std::make_shared<std::thread>(&Client::Start, client_);
        Loop();
    }

    void KvClient::Loop()
    {
        ShowHelp();

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
                client_->Send(command);
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

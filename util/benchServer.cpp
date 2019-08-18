#include "benchServer.hpp"
#include "../net/server.hpp"

#include <iostream>
#include <sstream>

namespace KvStoreServer{

    BenchServer::BenchServer(uint16_t port)
      : port_(port),
        server_(nullptr),
        thread_(nullptr)
    {}

    BenchServer::~BenchServer()
    {
        if(thread_)
        {
            thread_->join();
        }
    }

    void BenchServer::Start()
    {
        server_ = std::make_shared<Server>(port_);
        server_->SetRecvCallback(
            std::bind(&BenchServer::Receive, this, std::placeholders::_1, std::placeholders::_2)
            );

        thread_ = std::make_shared<std::thread>(&Server::Start, server_);
        Loop();
    }

    void BenchServer::Loop()
    {
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
                server_->Close();
                return;
            }
        }
    }

    void BenchServer::Receive(int sockfd, const std::string& message)
    {
        server_->Send(sockfd, message);
    }
}

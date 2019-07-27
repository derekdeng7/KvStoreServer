#include "kvServer.hpp"

#include <iostream>
#include <sstream>

namespace KvStoreServer{

    KvServer::KvServer(size_t threadNum, uint16_t port)
      : threadNum_(threadNum), 
        port_(port),
        server_(nullptr),
        threadPool_(nullptr),
        thread_(nullptr)
    {}

    KvServer::~KvServer()
    {
        threadPool_->Stop();
        delete db_;

        if(thread_)
        {
            thread_->join();
        }
    }

    void KvServer::Start()
    {
        leveldb::Options opts;
        opts.create_if_missing = true;
        leveldb::Status status = leveldb::DB::Open(opts, "./testdb", &db_);
        assert(status.ok());

        threadPool_ = std::make_shared<ThreadPool<EventCallback>>(threadNum_);
        threadPool_->Start();

        server_ = std::make_shared<Server>(port_);
        server_->SetRecvCallback(
            std::bind(&KvServer::Receive, this, std::placeholders::_1, std::placeholders::_2)
            );

        thread_ = std::make_shared<std::thread>(&Server::Start, server_);
        Loop();
    }

    void KvServer::Loop()
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
                exit(0);
            }
        }
    }

    void KvServer::Receive(int sockfd, const std::string& message)
    {
        threadPool_->AddTask(std::bind(&KvServer::HandleQuery, this, sockfd, message));
    }

    void KvServer::Send(int sockfd, const std::string& message)
    {
        server_->Send(sockfd, message);
    }

    void KvServer::HandleQuery(int sockfd, const std::string& message)
    {
        std::vector<std::string> argVec; 
         std::string arg;
        std::stringstream ss(message);
        leveldb::Status status;

        while(ss >> arg)
        {
            argVec.push_back(arg);
        }

        int argNum = argVec.size();
            
        if(!strcasecmp(argVec[0].c_str(), "put") && argNum == 3)
        {
            status = db_->Put(leveldb::WriteOptions(), argVec[1], argVec[2]);
            if(status.ok())
            {
                Send(sockfd, message + " successfully!");
            }                
            else
            {
                Send(sockfd, "fail to " + message );
            }

            return;
        }
            
        if(!strcasecmp(argVec[0].c_str(), "get") && argNum == 2)
        {
            std::string value;
            status = db_->Get(leveldb::ReadOptions(), argVec[1], &value);
            if(status.ok())
            {
                Send(sockfd, argVec[1] + " : " + value);
            }                
            else
            {
                Send(sockfd, "key " + argVec[1] + " does not exist" );
            }
            
            return;
        }

        if(!strcasecmp(argVec[0].c_str(), "delete") && argNum == 2)
        {
            status = db_->Delete(leveldb::WriteOptions(), argVec[1]);
            if(status.ok())
            {
                Send(sockfd, message + " successfully!");
            }                
            else
            {
                Send(sockfd, "fail to " + message );
            }

            return;
        }

        Send(sockfd, "Argument Error. Use \"help\" for help.");
        return; 
    }
}

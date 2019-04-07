#include "kvServer.hpp"


namespace KvStoreServer{

    KvServer::KvServer(size_t threadNum, uint16_t port)
      : threadNum_(threadNum), 
        port_(port),
        server_(nullptr),
        thread_(nullptr)
    {}

    KvServer::~KvServer()
    {
        if(thread_)
        {
            thread_->join();
        }
    }

    void KvServer::Start()
    {
        server_ = std::make_shared<Server>(threadNum_, port_);
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
}

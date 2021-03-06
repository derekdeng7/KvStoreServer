#ifndef _KVSTORESERVER_KVSTORE_KVCLIENT_HPP_
#define _KVSTORESERVER_KVSTORE_KVCLIENT_HPP_

#include "../net/client.hpp"
#include "../net/timeStamp.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

namespace KvStoreServer{

    class KvClient
    {
    public:
        KvClient();
        ~KvClient();

        void Start();
        int CreateConnection(const char* serverIp, uint16_t port);
        void Receive(int sockfd, const std::string& message);
        void Send(int sockfd, const std::string& message);
        void Loop(int sockfd);

    private:
        void ShowHelp();
        bool CheckCommand();

        std::vector<std::string> argVec_;
        std::shared_ptr<Client> client_;
        std::shared_ptr<std::thread> thread_;

        std::string message_;

        TimeStamp stamp_;
    };

}

#endif //_KVSTORESERVER_KVSTORE_KVCLIENT_HPP_

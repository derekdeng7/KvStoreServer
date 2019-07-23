#ifndef _KVSTORESERVER_KVSTORE_KVCLIENT_HPP_
#define _KVSTORESERVER_KVSTORE_KVCLIENT_HPP_

#include "../net/client.hpp"

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
        KvClient(int threadNum);
        ~KvClient();

        void Start();
        int CreateConnection(const char* serverIp, uint16_t port);
        void Receive(int sockfd, const std::string& message);
        
        void Send(int sockfd, const std::string& message);
        void SendEvery(double interval, int sockfd, const std::string& message);

        void StartBench(const char* serverIp, uint16_t port, size_t sessions, size_t messageNum, size_t messageSize);
        
        void Loop();
        void ShowHelp();
        bool CheckCommand();

    private:
        int threadNum_;

        std::vector<std::string> argVec_;
        std::shared_ptr<Client> client_;
        std::shared_ptr<std::thread> thread_;

        std::map<int, size_t> counts_;
        std::string message_;

        TimeStamp stamp_;
        size_t sessions_;
        size_t messageNum_;
    };

}

#endif //_KVSTORESERVER_KVSTORE_KVCLIENT_HPP_

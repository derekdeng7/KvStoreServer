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
        KvClient(const char* serverIp, uint16_t port);
        ~KvClient();

        void Start();
        void Loop();
        void ShowHelp();
        bool CheckCommand();

    private:
        const char* serverIP_;
        uint16_t port_;

        std::vector<std::string> argVec_;
        std::shared_ptr<Client> client_;
        std::shared_ptr<std::thread> thread_;
    };

}

#endif //_KVSTORESERVER_KVSTORE_KVCLIENT_HPP_

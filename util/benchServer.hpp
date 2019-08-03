#ifndef _KVSTORESERVER_KVSTORE_BENCHSERVER_HPP_
#define _KVSTORESERVER_KVSTORE_BENCHSERVER_HPP_

#include "../net/declear.hpp"

#include <vector>
#include <thread>

namespace KvStoreServer{

    class BenchServer
    {
    public:
        BenchServer(uint16_t port);
        ~BenchServer();

        void Start();
        void Loop();

        void Receive(int sockfd, const std::string& message);

    private:
        uint16_t port_;

        std::vector<std::string> argVec_;
        std::shared_ptr<Server> server_;
        std::shared_ptr<std::thread> thread_;
    };

}

#endif //_KVSTORESERVER_KVSTORE_BENCHSERVER_HPP_
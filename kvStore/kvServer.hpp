#ifndef _KVSTORESERVER_KVSTORE_KVSERVER_HPP_
#define _KVSTORESERVER_KVSTORE_KVSERVER_HPP_

#include "../net/server.hpp"

#include <vector>
#include <thread>

namespace KvStoreServer{

    class KvServer
    {
    public:
        KvServer(size_t threadNum, uint16_t port);
        ~KvServer();

        void Start();
        void Loop();

    private:
        size_t threadNum_;
        uint16_t port_;

        std::vector<std::string> argVec_;
        std::shared_ptr<Server> server_;
        std::shared_ptr<std::thread> thread_;
    };

}

#endif //_KVSTORESERVER_KVSTORE_KVCLIENT_HPP_

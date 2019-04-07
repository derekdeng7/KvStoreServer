#ifndef _KVSTORESERVER_NET_CLIENT_HPP_
#define _KVSTORESERVER_NET_CLIENT_HPP_

#include "socket.hpp"
#include "connector.hpp"
#include "declear.hpp"

namespace KvStoreServer{

    class Client
    {
    public:
        Client(const char* ip, uint16_t port);
        ~Client();

        void Start();
        void Close();
        void Send(const std::string& message);

    private:
        void WriteComplete();
        void RemoveConnection(int sockfd);

        std::unique_ptr<Socket> socket_;
        Address serverAddr_;
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<Connector> connector_;
    };
}

#endif // _KVSTORESERVER_NET_CLIENT_HPP_

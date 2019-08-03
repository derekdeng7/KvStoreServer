#ifndef _KVSTORESERVER_KVSTORE_BENCHBenchBenchClient_HPP_
#define _KVSTORESERVER_KVSTORE_BENCHBenchBenchClient_HPP_

#include "../net/declear.hpp"
#include "../include/callback.hpp"

#include <map>
#include <memory>
#include <thread>

namespace KvStoreServer{

    class BenchClient
    {
    public:
        BenchClient(const char* ip, uint16_t port, size_t sessions, size_t messageNum, size_t messageSize);
        ~BenchClient();

        void StartBench();
        void Close();

    private:
        int Connect(const char* ip, uint16_t port);
        void Send(int sockfd, const std::string& message);
        void Receive(int sockfd, const std::string& message);
        void NewConnection(std::shared_ptr<Socket> socket);
        void WriteComplete();
        void RemoveConnection(int sockfd);
        void RemoveConnectionInLoop(int sockfd);
        void ClearConnections();

        const char* ip_;
        uint16_t port_;
        std::shared_ptr<EventLoop> loop_;

        std::map<int, std::shared_ptr<Connector>> connections_;
    
        std::map<int, size_t> counts_;
        std::string message_;

        std::unique_ptr<TimeStamp> stamp_;
        size_t sessions_;
        size_t finishSessions_;
        size_t messageNum_;
        size_t messageSize_;
    };
}

#endif // _KVSTORESERVER_KVSTORE_BENCHBenchBenchClient_HPP_

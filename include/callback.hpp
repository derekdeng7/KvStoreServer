#ifndef _KVSTORESERVER_CALLBACK_HPP
#define _KVSTORESERVER_CALLBACK_HPP

#include "base.hpp"

#include <arpa/inet.h>
#include <functional>
#include <memory>
#include <string>

namespace KvStoreServer{

    typedef std::function<void()> EventCallback;
    typedef std::function<void(int sockfd, const sockaddr_in& addr)> NewConnectionCallback;

    typedef std::function<void(const std::string& message)> SendCallback;
    typedef std::function<void(int sockfd, std::string& message)> RecvCallback;
    typedef std::function<void()> WriteCompleteCallback;
    typedef std::function<void(int sockfd)> RemoveConnectionCallback;

    typedef std::function<bool(const KeyType& key, ValueType& value)> GetCallback;
    typedef std::function<void(const KeyType& key, const ValueType& value)> PutCallback;

}

#endif // _KVSTORESERVER_CALLBACK_HPP

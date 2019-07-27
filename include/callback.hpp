#ifndef _KVSTORESERVER_CALLBACK_HPP
#define _KVSTORESERVER_CALLBACK_HPP

#include "base.hpp"
#include "../net/declear.hpp"

#include <arpa/inet.h>
#include <functional>
#include <memory>
#include <string>

namespace KvStoreServer{

    typedef std::function<void()> EventCallback;
    typedef std::function<void()> TimerCallback;

    typedef std::function<void(const std::string& message)> SendCallback;
    typedef std::function<void(int sockfd, const std::string& message)> RecvCallback;
    typedef std::function<void()> WriteCompleteCallback;

    typedef std::function<void(std::shared_ptr<Socket> socket)> NewConnectionCallback;
    typedef std::function<void(int sockfd)> RemoveConnectionCallback;

}

#endif // _KVSTORESERVER_CALLBACK_HPP

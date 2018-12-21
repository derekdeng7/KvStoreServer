#ifndef _KVSTORESERVER_CALLBACK_HPP
#define _KVSTORESERVER_CALLBACK_HPP

#include <functional>
#include <memory>

namespace KvStoreServer{

    typedef std::function<void()> EventCallback;
    //typedef std::shared_ptr<Connector> ConnectorPtr;
    //typedef std::function<void(const ConnectorPtr&)> ConnectCallback;
    //typedef std::function<void()> ReadCallback;
    typedef std::function<void()> WriteCompleteCallback;
    typedef std::function<void(int sockdf)> CloseConnectionCallback;
}

#endif // _KVSTORESERVER_CALLBACK_HPP
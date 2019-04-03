#ifndef _KVSTORESERVER_NET_TASK_HPP_
#define _KVSTORESERVER_NET_TASK_HPP_

#include "buffer.hpp"
#include "../include/callback.hpp"
#include "declear.hpp"

#include <iostream>
#include <string>
#include <functional>

namespace KvStoreServer{

    class Task
    {
    public:
        Task()
        {}

        Task(SendCallback sendCallback, Message message)
          : sendCallback_(sendCallback),
            message_(message)
        {}

        virtual ~Task()
        {}

        void virtual processTask() = 0;

    protected:
        SendCallback sendCallback_;
        Message message_;
    };


    class TaskInEventLoop : public Task
    {
    public:
        TaskInEventLoop(RemoveConnectionCallback callback, int sockfd)
          : Task(),
            removeConnectionCallback_(callback),
            sockfd_(sockfd)
        {}

        TaskInEventLoop(SendCallback callback, Message message)
          : Task(callback, message)
        {}

        void virtual processTask()
        {
            if(sendCallback_)
            {
                sendCallback_(message_);
            }
            else if(removeConnectionCallback_)
            {
                removeConnectionCallback_(sockfd_);
            }
        }

    private:
        RemoveConnectionCallback removeConnectionCallback_;
        int sockfd_;
    };

    class TaskInSyncQueue : public Task
    {
    public:
        TaskInSyncQueue(SendCallback callback, Message message)
          : Task(callback, message)
        {}

        void SetCallback(const GetCallback& gcallback, const PutCallback& pcallback)
        {
            getCallback_ = gcallback;
            putCallback_ = pcallback;
        }

        void virtual processTask()
        {
            if(sendCallback_)
            {
                if(message_.option == 1)
                {
                    std::cout << "get value of key:" << message_.entry.internalKey.key << std::endl;
                    message_.flag = getCallback_(message_.entry.internalKey, message_.entry.value);
                    sendCallback_(message_);
                }
                else if(message_.option == 2)
                {
                    std::cout << "put key-value: " << message_.entry.internalKey.key << "-" << message_.entry.value.str << std::endl;
                    putCallback_(message_.entry);
                }
            }
        }

    private:
        GetCallback getCallback_;
        PutCallback putCallback_;
    };

}

#endif //_KVSTORESERVER_NET_TASK_HPP_

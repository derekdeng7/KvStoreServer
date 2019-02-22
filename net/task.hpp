#ifndef _KVSTORESERVER_TASK_HPP_
#define _KVSTORESERVER_TASK_HPP_

#include <string>
#include <functional>

#include "buffer.hpp"
#include "callback.hpp"
#include "declear.hpp"
#include "define.hpp"

namespace KvStoreServer{

    class Task
    {
    public:
        Task();
        Task(SendCallback sendCallback, std::string message);
        void virtual processTask() = 0;

    protected:
        SendCallback sendCallback_;
        std::string message_;
    };


    class TaskInEventLoop : public Task
    {
    public:
        TaskInEventLoop(RemoveConnectionCallback callback, int sockfd);
        TaskInEventLoop(SendCallback callback, std::string message);
        void virtual processTask();

    private:
        RemoveConnectionCallback removeConnectionCallback_;
        int sockfd_;
    };

    class TaskInSyncQueue : public Task
    {
    public:
        TaskInSyncQueue(SendCallback callback, std::string message);
        void virtual processTask();

    };

}

#endif //_KVSTORESERVER_TASK_HPP_

#ifndef _KVSTORESERVER_TASK_HPP_
#define _KVSTORESERVER_TASK_HPP_

#include <string>
#include <functional>

#include "connector.hpp"

namespace KvStoreServer{

    class Task
    {
    public:
        Task(std::shared_ptr<Connector> pConnector, std::string message);
        std::shared_ptr<Connector> getConnector() const;
        std::string getMessage() const;
        void virtual processTask() = 0;

    private:
        std::shared_ptr<Connector> pConnector_;
        std::string message_;
    };


    class TaskInEventLoop : public Task
    {
    public:
        TaskInEventLoop(std::shared_ptr<Connector> pConnector);
        TaskInEventLoop(std::shared_ptr<Connector> pConnector, std::string message);
        void virtual processTask();

    };

    class TaskInSyncQueue : public Task
    {
    public:
        TaskInSyncQueue(std::shared_ptr<Connector> pConnector, std::string message);
        void virtual processTask();

    };

}

#endif //_KVSTORESERVER_TASK_HPP_
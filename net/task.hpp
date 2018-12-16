#ifndef _NETWORK_TASK_HPP_
#define _NETWORK_TASK_HPP_

#include <string>
#include <functional>

#include "connector.hpp"

namespace Network{

    class Task
    {
    public:
        Task(Connector* pConnector, std::string message);
        Connector* getConnector() const;
        std::string getMessage() const;
        void virtual processTask() = 0;

    private:
        Connector* pConnector_;
        std::string message_;
    };


    class TaskInEventLoop : public Task
    {
    public:
        TaskInEventLoop(Connector* pConnector);
        TaskInEventLoop(Connector* pConnector, std::string message);
        void virtual processTask();

    };

    class TaskInSyncQueue : public Task
    {
    public:
        TaskInSyncQueue(Connector* pConnector, std::string message);
        void virtual processTask();

    };

}

#endif //_NETWORK_TASK_HPP_
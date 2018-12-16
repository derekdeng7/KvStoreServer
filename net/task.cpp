#include "task.hpp"

namespace Network{

    Task::Task(Connector* pConnector, std::string message) :
        pConnector_(pConnector), message_(message)
    {

    }

    Connector* Task::getConnector() const
    {
        return this->pConnector_;
    }
    std::string Task::getMessage() const
    {
        return this->message_;
    }
    
    TaskInEventLoop::TaskInEventLoop(Connector* pConnector) : 
        Task(pConnector, "NULL")
    {

    }

    TaskInEventLoop::TaskInEventLoop(Connector* pConnector, std::string message) :
        Task(pConnector, message)
    {

    }

    void TaskInEventLoop::processTask()
    {
        Connector* pConnector = getConnector();
        std::string message = getMessage();
        if(message == "NULL")
        {
            pConnector->WriteComplete();
        }
        else
        {
            pConnector->SendInLoop(message);
        }
    }

    TaskInSyncQueue::TaskInSyncQueue(Connector* pConnector, std::string message) :
        Task(pConnector, message)
    {

    }

    void TaskInSyncQueue::processTask()
    {
        Connector* pConnector = getConnector();
        std::string message = getMessage();

        //do anything you want!!!
        
        message = message + " from thread id:" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        Buffer tmpBuf;
        tmpBuf.Append(message);
        std::string str;
        while(tmpBuf.DataSize() > 0)
        {
            if(tmpBuf.DataSize() >= MESSAGE_SIZE)
            {
                str = tmpBuf.RetriveAsString(MESSAGE_SIZE);
            }
            else
            {
                str = tmpBuf.RetriveAllAsString();
            }
            pConnector->Send(str);
                
        }
    }

}
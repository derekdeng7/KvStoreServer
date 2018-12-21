#include <thread>
#include "task.hpp"

namespace KvStoreServer{

    Task::Task()
    {}

    Task::Task(SendCallback sendCallback, std::string message) :
        sendCallback_(sendCallback), message_(message)
    {}
    
    TaskInEventLoop::TaskInEventLoop(RemoveConnectionCallback callback, int sockfd) : 
        Task(),
        removeConnectionCallback_(callback),
        sockfd_(sockfd)
    {}

    TaskInEventLoop::TaskInEventLoop(SendCallback callback, std::string message) :
        Task(callback, message)
    {}

    void TaskInEventLoop::processTask()
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

    TaskInSyncQueue::TaskInSyncQueue(SendCallback callback, std::string message) :
        Task(callback, message)
    {}

    void TaskInSyncQueue::processTask()
    {
        if(sendCallback_)
        {
            //do anything you want!!!
            message_ = message_ + " from thread id:" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            Buffer tmpBuf;
            tmpBuf.Append(message_);
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
                sendCallback_(str);
                    
            }
        }

        
    }

}
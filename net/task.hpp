#ifndef _KVSTORESERVER_NET_TASK_HPP_
#define _KVSTORESERVER_NET_TASK_HPP_

#include <cstdlib>
#include <string>
#include <functional>
#include <sstream>
#include <vector>

#include "buffer.hpp"
#include "declear.hpp"
#include "../include/callback.hpp"
#include "../db/lsmTree.hpp"

namespace KvStoreServer{

    class Task
    {
    public:
        Task()
        {}

        Task(SendCallback sendCallback, std::string message)
          : sendCallback_(sendCallback), message_(message)
        {}

        void virtual processTask() = 0;

    protected:
        SendCallback sendCallback_;
        std::string message_;
    };


    class TaskInEventLoop : public Task
    {
    public:
        TaskInEventLoop(RemoveConnectionCallback callback, int sockfd)
          : Task(),
            removeConnectionCallback_(callback),
            sockfd_(sockfd)
        {}

        TaskInEventLoop(SendCallback callback, std::string message)
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
        TaskInSyncQueue(SendCallback callback, std::string message)
          : Task(callback, message)
        {}

        void SetCallback(GetCallback getCallback, PutCallback putCallback)
        {
            getCallback_ = getCallback;
            putCallback_ = putCallback;
        }

        void virtual processTask()
        {
            if(!sendCallback_)
            {
                return;
            }
            
            sendCallback_(ProcessTask());
        }

        std::string ProcessTask()
        {
            std::vector<std::string> argVec; 
            std::string arg;
            std::stringstream ss(message_);

            while(ss >> arg)
            {
                argVec.push_back(arg);
            }

            int argNum = argVec.size();
            LSMTree* lsmTree = LSMTree::getInstance();
            
            if(!strcasecmp(argVec[0].c_str(), "put") && argNum == 3)
            {
                lsmTree->Put(atoi(argVec[1].c_str()), argVec[2].c_str());
                return message_ + " successfully!";
            }
            
            if(!strcasecmp(argVec[0].c_str(), "get") && argNum == 2)
            {
                ValueType value;
                bool flag = lsmTree->Get(atoi(argVec[1].c_str()), value);
                if(!flag)
                {
                    return "key " + argVec[1] + " does not exist!";
                }
                
                return message_ + ":" + value.str;
            }
            
            if(!strcasecmp(argVec[0].c_str(), "update") && argNum == 3)
            {
                lsmTree->Put(atoi(argVec[1].c_str()), argVec[2].c_str());
                return message_ + " successfully!";
            }
            
            if(!strcasecmp(argVec[0].c_str(), "remove") && argNum == 2)
            {
                lsmTree->Remove(atoi(argVec[1].c_str()));
                return message_ + " successfully!";
            }

            return std::string("Argument Error. Use \"help\" for help.");
        }

    private:
        GetCallback getCallback_;
        PutCallback putCallback_;
    };

}

#endif //_KVSTORESERVER_NET_TASK_HPP_

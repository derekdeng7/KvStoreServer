#ifndef _KVSTORESERVER_DB_LOG_HPP_
#define _KVSTORESERVER_DB_LOG_HPP_

#include "../include/base.hpp"
#include "../include/fileOperator.hpp"
#include "../include/syncThread.hpp"

#include <atomic>
#include <functional>
#include <queue>

namespace KvStoreServer{

    class Log : public SyncThread<Entry>
    {
    public:
        typedef std::function<void(const Entry& entry)> InsertCallback;

        Log()
          : SyncThread<Entry>(),
            entryNum_(0)
        {}

        bool Start();
        bool Write(const Entry& entry);
        
        void Update()
        {
            remove(pathQueue_.front().c_str());
            pathQueue_.pop(); 
        }

        void SetInsertCallback(const InsertCallback& callback)
        {
            insertCallback_ = callback;
        }

    private:
        bool CreateActiveLog();
        bool CreateFrozenLog();
        bool Recovery();

        virtual void ProcessTask(const Entry& entry)
        {
            Write(entry);
        }

        bool ReadLogMeta()
        {
            FileOperator fp("rb", LOGMETAPATH);
            return fp.Read(&meta_, 0, sizeof(LogMeta));
        }

        LogMeta meta_;
        size_t entryNum_;
        InsertCallback insertCallback_;
        std::queue<std::string> pathQueue_;
    };
}

#endif //_KVSTORESERVER_DB_LOG_HPP_

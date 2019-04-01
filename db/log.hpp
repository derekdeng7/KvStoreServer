#ifndef _KVSTORESERVER_DB_LOG_HPP_
#define _KVSTORESERVER_DB_LOG_HPP_

#include "base.hpp"
#include "fileOperator.hpp"

#include <functional>

namespace KvStoreServer{

    class Log
    {
    public:
        typedef std::function<void(const Entry& entry)> InsertCallback;

        Log()
        {}

        bool Init();
        bool Write(Entry& entry);
        bool CreateFrozenLog();
        bool RemoveFrozenLog()
        {
            return !remove(meta_.frozenLogPath);
        }

        void SetInsertCallback(const InsertCallback& callback)
        {
            insertCallback_ = callback;
        }

    private:
        bool CreateActiveLog();
        bool Recovery();
        bool ReadLogMeta()
        {
            FileOperator fp("rb");
            return fp.Read(&meta_, sizeof(LsmTreeMeta) + sizeof(LevelMeta) * MAXHLEVELNUM, sizeof(LogMeta));
        }

        LogMeta meta_;
        InsertCallback insertCallback_;
    };
}

#endif //_KVSTORESERVER_DB_LOG_HPP_

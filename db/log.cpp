#include "log.hpp"

#include <vector>
#include <thread>

namespace KvStoreServer{

    bool Log::Start()
    {
        StartThread();

        if(!ReadLogMeta())
        {
            return CreateActiveLog();
        }
        
        return Recovery();
    }

    bool Log::Write(const Entry& entry)
    {
        if(entryNum_ >= MAXENTRYNUM)
        {
            CreateFrozenLog();
        }

        FileOperator fp("rb+", meta_.activeLogPath);
        if(!fp.Write((Entry*)&entry, (entryNum_++) * sizeof(Entry), sizeof(Entry)))
        {
            return false;
        }

        return true;
    }
  
    bool Log::CreateActiveLog()
    {
        FileOperator fp1("rb+", LOGMETAPATH);
        SeqType st;
        std::string str = ".log/" + std::to_string(st.seq)  + ".lg";
        pathQueue_.push(str);
        strcpy(meta_.activeLogPath, str.c_str());
        //meta_.activeLogEntryNum = 0;
        FileOperator fp2("w+", meta_.activeLogPath);
        
        entryNum_ = 0;
        
        return fp1.Write(&meta_, 0, sizeof(LogMeta));
    }

    bool Log::CreateFrozenLog()
    {
        //remove(meta_.frozenLogPath);

        strcpy(meta_.frozenLogPath, meta_.activeLogPath);
        //meta_.frozenLogEntryNum = meta_.activeLogEntryNum;
        
        return CreateActiveLog();
    }

    bool Log::Recovery()
    {   
        Entry entry;
        off_t offset = 0;

        FileOperator fp1("rb", meta_.frozenLogPath);
        while(fp1.Read(&entry, offset, sizeof(Entry)))
        {
            insertCallback_(entry);
            offset += sizeof(Entry);
        }

        FileOperator fp2("rb", meta_.activeLogPath);
        offset = 0;
        while(fp2.Read(&entry, offset, sizeof(Entry)))
        {
            insertCallback_(entry);
            offset += sizeof(Entry);
        }

        return true;
    }

}

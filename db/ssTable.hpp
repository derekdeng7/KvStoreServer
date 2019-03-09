#ifndef _KVSTORESERVER_DB_SSTABLE_HPP_
#define _KVSTORESERVER_DB_SSTABLE_HPP_

#include "entry.hpp"

#include <vector>

namespace KvStoreServer{

    struct SSMeta
    {
        size_t entryNum;
        KeyType minKey;
        KeyType maxKey;
    };
    
    class SSTable
    {
    public:
        SSTable(){}

        //only call when the immuable memtable flush into disk
        SSTable(std::vector<Entry> entryVec) 
          : entryVec_(entryVec) 
        {
            InitFormMemTable();
        }
        
        void InitFormMemTable();
        bool LoadFromDisk();
        bool MapRead(const KeyType& key);
        void ShowData() const;

    private:
        bool FlushInDisk();
        char path_[64];
        SSMeta meta_;
        std::vector<Entry> entryVec_;
    };
}

#endif //_KVSTORESERVER_DB_SSTABLE_HPP_


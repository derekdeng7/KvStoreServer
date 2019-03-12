#ifndef _KVSTORESERVER_DB_SSTABLE_HPP_
#define _KVSTORESERVER_DB_SSTABLE_HPP_

#include "entry.hpp"

#include <vector>

namespace KvStoreServer{
 
    class SSTable
    {
    public:
        //load from disk and search key
        SSTable(const SSTableMeta& meta) 
          : meta_(meta) 
        {}

        //only call when the immuable memtable dump in disk
        SSTable(std::vector<Entry> entryVec, off_t prev) 
          : entryVec_(entryVec) 
        {
            meta_.prev = prev;
        }
        
        bool SearchFromDisk(const KeyType& key, ValueType& value);
        bool WriteInDisk();
        void ShowData() const;
        SSTableMeta GetMeta() const
        {
            return this->meta_;
        }

    private:
        SSTableMeta meta_;
        std::vector<Entry> entryVec_;
    };
}

#endif //_KVSTORESERVER_DB_SSTABLE_HPP_


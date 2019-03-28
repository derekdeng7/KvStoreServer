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
        SSTable(std::vector<Entry> entryVec) 
          : entryVec_(entryVec) 
        {
        }
        
        bool SearchFromDisk(const KeyType& key, ValueType& value);
        bool WriteInDisk();
        std::vector<Entry> GetData();

        SSTableMeta GetMeta() const
        {
            return this->meta_;
        }

        bool Remove()
        {
            if(!remove(meta_.filePath))
            {
                perror("fail to remove SSTable");
                return false;
            }

            return true;
        }

    private:
        SSTableMeta meta_;
        std::vector<Entry> entryVec_;
    };
}

#endif //_KVSTORESERVER_DB_SSTABLE_HPP_


#ifndef _KVSTORESERVER_DB_SSTABLE_HPP_
#define _KVSTORESERVER_DB_SSTABLE_HPP_

#include "base.hpp"

#include <vector>

namespace KvStoreServer{
 
    struct Index
    {
        KeyType internalKey;
        off_t offset;

        Index()
          :offset(0)
        {}

        Index(KeyType key, off_t off)
          :internalKey(key), offset(off)
        {}
    };

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
        {}
        
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
        Index indexArray_[32];
    };
}

#endif //_KVSTORESERVER_DB_SSTABLE_HPP_


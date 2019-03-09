#ifndef _KVSTORESERVER_DB_MEMTABLE_HPP_
#define _KVSTORESERVER_DB_MEMTABLE_HPP_

#include "skipList.hpp"

namespace KvStoreServer{

    class MemTable
    {
    public:
        MemTable(size_t maxHeight, size_t maxEntryNum) 
          : maxHeight_(maxHeight), maxEntryNum_(maxEntryNum), table_(maxHeight_, maxEntryNum_) 
        {}

        bool Search(const KeyType& key, ValueType& value)
        {
            return table_.Search(key, value);
        }

        void Insert(const KeyType& key, const ValueType& value)
        {
            table_.Insert(key, value);
        }

        void ShowData() const
        {
            table_.ShowData();
        }

        std::vector<Entry> PopAllEntries()
        { 
            return table_.PopAllEntries();
        }

        size_t GetLevelNum() const 
        { 
            return table_.GetHeight(); 
        }

        size_t GetEntryNum() const 
        { 
            return table_.GetEntryNum();
        }
    
    private:
        size_t maxHeight_;
        size_t maxEntryNum_;
        SkipList table_;
    };
}

#endif //_KVSTORESERVER_DB_MEMTABLE_HPP_

#ifndef _KVSTORESERVER_DB_LSMTREE_HPP_
#define _KVSTORESERVER_DB_LSMTREE_HPP_

#include "compaction.hpp"
#include "level.hpp"
#include "log.hpp"
#include "memTable.hpp"

#include <cassert>
#include <thread>
#include <vector>

namespace KvStoreServer{

    class LSMTree   
    {
    public:
        LSMTree();
        bool InitFromEmpty();
        bool InitFromFile();
        bool Get(const KeyType& key, ValueType& value);
        void Put(const KeyType& key, const ValueType& value);
    
        void Remove(const KeyType& key)
        {
            Put(key, DELETETAG);
        }

    private:
        bool ReadLSMTreeMeta()
        {
            FileOperator fp("rb");
            return fp.Read(&meta_, 0, sizeof(LsmTreeMeta));
        }

        void Insert(const Entry& entry);
        
        LsmTreeMeta meta_;
        std::unique_ptr<Log> log_;
        std::unique_ptr<MemTable> muTable_;
        std::unique_ptr<MemTable> immuTable_;
        std::unique_ptr<Compaction> compaction_;
    };
}

#endif //_KVSTORESERVER_DB_LSMTREE_HPP_

#ifndef _KVSTORESERVER_DB_LSMTREE_HPP_
#define _KVSTORESERVER_DB_LSMTREE_HPP_

#include "compaction.hpp"
#include "level.hpp"
#include "memTable.hpp"

#include <cassert>
#include <thread>
#include <vector>

namespace KvStoreServer{

    class LSMTree   
    {
    public:
        LSMTree() 
          : muTable_(new MemTable(MAXHEIGHT)), immuTable_(new MemTable(MAXHEIGHT))
        {
            InitFromFile();
            compaction_ = std::unique_ptr<Compaction>(new Compaction(meta_.levelNum));
        }

        bool InitFromFile();
        bool InitFromEmpty();
        bool Get(const KeyType& key, ValueType& value);
        void Put(const KeyType& key, const ValueType& value);
    
        void Remove(const KeyType& key)
        {
            Put(key, DELETETAG);
        }

    private:
        bool ReadLSMTreeMeta();
        
        LsmTreeMeta meta_;
        std::unique_ptr<MemTable> muTable_;
        std::unique_ptr<MemTable> immuTable_;
        std::unique_ptr<Compaction> compaction_;
    };
}

#endif //_KVSTORESERVER_DB_LSMTREE_HPP_

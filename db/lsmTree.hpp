#ifndef _KVSTORESERVER_DB_LSMTREE_HPP_
#define _KVSTORESERVER_DB_LSMTREE_HPP_

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
        bool UpdateLSMTreeMeta();
        void FlushToDisk();
        
        LsmTreeMeta meta_;
        std::vector<Level> levelVec_;
        std::unique_ptr<MemTable> muTable_;
        std::unique_ptr<MemTable> immuTable_;
    };
}

#endif //_KVSTORESERVER_DB_LSMTREE_HPP_

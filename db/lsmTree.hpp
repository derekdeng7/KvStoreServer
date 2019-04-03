#ifndef _KVSTORESERVER_DB_LSMTREE_HPP_
#define _KVSTORESERVER_DB_LSMTREE_HPP_

#include "compaction.hpp"
#include "level.hpp"
#include "log.hpp"
#include "memTable.hpp"
#include "../include/callback.hpp"

#include <cassert>
#include <vector>

namespace KvStoreServer{

    class LSMTree : public SyncThread<Entry>   
    {
    public:
        LSMTree()
          : SyncThread<Entry>(),
            log_(new Log()), 
            muTable_(new MemTable(MAXHEIGHT)), 
            immuTable_(new MemTable(MAXHEIGHT))
        {}

        ~LSMTree()
        {
            Stop();
        }

        void Start()
        {
            InitFromFile();
            compaction_ = std::unique_ptr<Compaction>(new Compaction(meta_.levelNum));
            
            StartThread();
        }

        bool InitFromFile();
        bool Get(const KeyType& key, ValueType& value);
        void Put(const Entry& entry);
    
    private:
        bool InitFromEmpty();

        virtual void ProcessTask(const Entry& entry)
        {
            return Insert(entry);
        }

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

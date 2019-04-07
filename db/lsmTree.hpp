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
        static LSMTree* getInstance()
        {
            static LSMTree instance;
            return &instance;
        }

        void Start();
        bool Get(const KeyType& key, ValueType& value);
        void Put(const KeyType& key, const ValueType& value);
        void Remove(const KeyType& key);

    private:
        struct ObjectCreator
        {
            ObjectCreator()
            {
                LSMTree::getInstance();
            }
        };
        static ObjectCreator objectCreator_;

        LSMTree();
        ~LSMTree();
        LSMTree(const LSMTree&) = delete;
        LSMTree& operator=(const LSMTree&) = delete;

        bool InitFromFile();
        bool InitFromEmpty();
        bool ReadLSMTreeMeta();
        void Insert(const Entry& entry);
        virtual void ProcessTask(const Entry& entry);
        
        LsmTreeMeta meta_;
        std::unique_ptr<Log> log_;
        std::unique_ptr<MemTable> muTable_;
        std::unique_ptr<MemTable> immuTable_; 
        std::unique_ptr<Compaction> compaction_;
    };
}

#endif //_KVSTORESERVER_DB_LSMTREE_HPP_

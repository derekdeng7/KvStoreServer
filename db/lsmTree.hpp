#ifndef _KVSTORESERVER_DB_LSMTREE_HPP_
#define _KVSTORESERVER_DB_LSMTREE_HPP_

#include "memTable.hpp"
#include "ssTable.hpp"

namespace KvStoreServer{

    const char* DELETETAG = "ENTRYHASBEENDEL";

    class LSMTree   
    {
    public:
        LSMTree() 
          : maxHeight_(15), maxEntryNum_(4096), muTable_(maxHeight_, maxEntryNum_), immuTable_(maxHeight_, maxEntryNum_)
        {}

        bool Get(const KeyType& key, ValueType& value)
        {
            if(muTable_.Search(key, value))
            { 
                return true;
            }
            else if(immuTable_.Search(key, value))
            { 
                return true;
            }
            else
            {
                return false;
            }
        }

        void Put(const KeyType& key, const ValueType& value)
        {
            muTable_.Insert(key, value);
        }


        void Remove(const KeyType& key)
        {
            muTable_.Insert(key, DELETETAG);
        }

        void ShowData()
        {
            muTable_.ShowData();
        }

        std::vector<Entry> PopAllEntries()
        { 
            return muTable_.PopAllEntries();
        }

        void FlushInDisk()
        {
            SSTable ssTable(PopAllEntries());
        }

        void LoadFromDisk()
        {
            SSTable ssTable;
            ssTable.LoadFromDisk();
            ssTable.ShowData();
        }

    private:
        //void buildImmuTable();
        //void buildSSTable();
        //void loadSSTable();

        size_t maxHeight_;
        size_t maxEntryNum_;
        MemTable muTable_;
        MemTable immuTable_;
    };
}

#endif //_KVSTORESERVER_DB_LSMTREE_HPP_

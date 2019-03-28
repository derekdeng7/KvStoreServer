#include "lsmTree.hpp"
#include "fileOperator.hpp"

#include <iostream>

namespace KvStoreServer{

    bool LSMTree::InitFromFile()
    {
        FileOperator fp("rb");
        if(!fp.Read(&meta_, 0, sizeof(LsmTreeMeta)))
        {
            return InitFromEmpty();
        }

        return true;
    }

    bool LSMTree::InitFromEmpty()
    {
        FileOperator fp("wb+");

        off_t offset = 0;
        //write LsmTreeMeta
        meta_.slot = sizeof(LsmTreeMeta) + MAXHLEVELNUM * sizeof(LevelMeta);
        if(!fp.Write(&meta_, 0, sizeof(LsmTreeMeta)))
        {
            return false;
        }

        //write LevelMeta
        offset += sizeof(LsmTreeMeta);
        for(auto i = 0; i < MAXHLEVELNUM; i++, offset += sizeof(LevelMeta))
        {
            LevelMeta levelMeta(i);
            if(!fp.Write(&levelMeta, offset, sizeof(LevelMeta)))
            {
                return false;
            }
        }
        
        return true;
    }

    bool LSMTree::Get(const KeyType& key, ValueType& value)
    {            
        if(muTable_->Search(key, value))
        { 
            return true;
        }   
        else if(immuTable_->Search(key, value))
        { 
            return true;            
        }
        else
        {
            //Search level
            for(size_t i = 0; i < meta_.levelNum; i++)
            {
                Level level(i);
                if(level.Search(key, value))
                {
                    return true;
                }
            }
            return false;
        } 
    }

    void LSMTree::Put(const KeyType& key, const ValueType& value)
    {
        muTable_->Insert(key, value);

        if(muTable_->GetEntryNum() >= MAXENTRYNUM)
        {
            //flush immutable to disk
            if(immuTable_->GetEntryNum() != 0)
            {
                //Compaction cp(meta_.levelNum);
                compaction_->MinorCompaction(std::move(immuTable_));
                ReadLSMTreeMeta();
            }

            immuTable_ = std::move(muTable_);

            assert(muTable_ == NULL);
            muTable_ = std::unique_ptr<MemTable>(new MemTable(MAXHEIGHT));
        }
    }

    bool LSMTree::ReadLSMTreeMeta()
    {
        FileOperator fp("rb");
        return fp.Read(&meta_, 0, sizeof(LsmTreeMeta));
    }

}

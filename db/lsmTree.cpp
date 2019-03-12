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

        //read LevelMeta
        size_t N = meta_.levelNum;
        for(size_t i = 0; i < N; i++)
        {
            Level level(i);
            levelVec_.push_back(std::move(level));
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
        LevelMeta levelMeta;
        for(auto i = 0; i < MAXHLEVELNUM; i++, offset += sizeof(LevelMeta))
        {
            if(!fp.Write(&levelMeta, offset, sizeof(LevelMeta)))
            {
                return false;
            }
        }
        
        Level level(0);
        levelVec_.push_back(std::move(level));

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
            for(auto iter : levelVec_)
            {
                if(iter.Search(key, value))
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
                FlushToDisk();
            }

            immuTable_ = std::move(muTable_);

            assert(muTable_ == NULL);
            muTable_ = std::unique_ptr<MemTable>(new MemTable(MAXHEIGHT));
        }
    }

    bool LSMTree::UpdateLSMTreeMeta()
    {
        FileOperator fp("rb+");
        return fp.Write(&meta_, 0, sizeof(LsmTreeMeta));
    }

    void LSMTree::FlushToDisk()
    {
        assert(immuTable_->GetEntryNum() == MAXENTRYNUM);

        SSTable newTable(immuTable_->PopAllEntries(), levelVec_[0].GetLastSSTableOffset());

        newTable.WriteInDisk();

        levelVec_[0].UpdateMeta(newTable.GetMeta(), meta_.slot); 
        meta_.slot += sizeof(SSTableMeta);
        UpdateLSMTreeMeta();
    } 

}

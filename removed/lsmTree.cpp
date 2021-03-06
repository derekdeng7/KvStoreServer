#include "lsmTree.hpp"

#include <iostream>
#include <functional>

namespace KvStoreServer{

    LSMTree::ObjectCreator LSMTree::objectCreator_;

    LSMTree::LSMTree()
      : SyncThread<Entry>(),
        log_(new Log()), 
        muTable_(new MemTable(MAXHEIGHT)), 
        immuTable_(new MemTable(MAXHEIGHT))
    {}

    LSMTree::~LSMTree()
    {
        Stop();
    }

    void LSMTree::Start()
    {
        InitFromFile();
        compaction_ = std::unique_ptr<Compaction>(new Compaction(meta_.levelNum));
        
        StartThread();
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
        for(size_t i = 0; i < MAXHLEVELNUM; i++, offset += sizeof(LevelMeta))
        {
            LevelMeta levelMeta(i);
            if(!fp.Write(&levelMeta, offset, sizeof(LevelMeta)))
            {
                return false;
            }
        }

        FileOperator fp1("wb+", LOGMETAPATH);

        return true;
    }

    bool LSMTree::InitFromFile()
    {
        if(!ReadLSMTreeMeta())
        {
            InitFromEmpty();
        }

        log_->SetInsertCallback(
            std::bind(&LSMTree::Insert, this, std::placeholders::_1)
        );

        return log_->Start();
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
        Entry entry(key, value);
        log_->AddTask(entry);
        AddTask(entry);
    }

    void LSMTree::Remove(const KeyType& key)
    {
        Put(key, DELETETAG);
    }

    bool LSMTree::ReadLSMTreeMeta()
    {
        FileOperator fp("rb");
        return fp.Read(&meta_, 0, sizeof(LsmTreeMeta));
    }

    void LSMTree::Insert(const Entry& entry)
    {
        muTable_->Insert(entry);

        if(muTable_->GetEntryNum() >= MAXENTRYNUM)
        {
            //flush immutable to disk
            if(immuTable_->GetEntryNum() != 0)
            {
                compaction_->MinorCompaction(std::move(immuTable_));
                log_->Update();
                ReadLSMTreeMeta();
            }

            immuTable_ = std::move(muTable_);

            assert(muTable_ == NULL);
            muTable_ = std::unique_ptr<MemTable>(new MemTable(MAXHEIGHT));
        }
    }

    void LSMTree::ProcessTask(const Entry& entry)
    {
        return Insert(entry);
    }
}

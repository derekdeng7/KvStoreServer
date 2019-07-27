#include "level.hpp"
#include "../include/fileOperator.hpp"

namespace KvStoreServer{

    bool Level::WriteLevelMeta()
    {
        assert(levelNo_ < MAXHLEVELNUM); 
        
        FileOperator fp("rb+");

        off_t offset = sizeof(LsmTreeMeta) + levelNo_ * sizeof(LevelMeta);
        return fp.Write(&meta_, offset, sizeof(LevelMeta));
    }

    bool Level::ReadLevelMeta()
    {
        assert(levelNo_ < MAXHLEVELNUM);

        FileOperator fp("rb+");

        off_t offset = sizeof(LsmTreeMeta) + levelNo_ * sizeof(LevelMeta);
        return fp.Read(&meta_, offset, sizeof(LevelMeta));
    }

    bool Level::Search(const KeyType& key, ValueType& value)
    {
        if(meta_.minKey > key || meta_.maxKey < key)
        {
            return false;
        }

        FileOperator fp("rb");
        
        size_t N = meta_.ssTableNum;
        off_t offset = meta_.header.next;
        for(size_t i = 0; i < N; i++)
        {
            SSTableMeta sstMeta;
            if(!fp.Read(&sstMeta, offset, sizeof(SSTableMeta)))
            {
                return false;
            }

            if(sstMeta.minKey < key && key < sstMeta.maxKey)
            {
                SSTable sst(sstMeta); 
                if(sst.SearchFromDisk(key, value))
                {
                    return true;
                }
            }

            offset = sstMeta.next;
        }

        return false;
    }
        

}

#include "level.hpp"
#include "fileOperator.hpp"

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

    bool Level::UpdateMeta(SSTableMeta sstMeta, off_t newSSTMetaOffset)
    {
        FileOperator fp("rb+");

        //update LevelMeta.lastSSTable
        off_t offset = meta_.lastSSTable + sizeof(SSTableMeta) - sizeof(off_t); 
        if(!fp.Write(&newSSTMetaOffset, offset, sizeof(off_t)))
        {
            return false;
        }

        //update LevelMeta
        meta_.ssTableNum++;
        if(meta_.minKey > sstMeta.minKey)
        {
            meta_.minKey = sstMeta.minKey;
        }

        if(meta_.maxKey < sstMeta.maxKey)
        {
            meta_.maxKey = sstMeta.maxKey;
        }

        if(meta_.firstSSTable == 0)
        {
            meta_.firstSSTable = newSSTMetaOffset;
        }

        meta_.lastSSTable = newSSTMetaOffset;
        
        offset = sizeof(LsmTreeMeta);
        if(!fp.Write(&meta_, offset, sizeof(LevelMeta)))
        {
            return false;
        }

        //add SSTableMeta 
        offset = newSSTMetaOffset;
        return fp.Write(&sstMeta, offset, sizeof(SSTableMeta));
    }

    bool Level::Search(const KeyType& key, ValueType& value)
    {
        if(meta_.minKey > key || meta_.maxKey < key)
        {
            return false;
        }

        FileOperator fp("rb");
        
        if(levelNo_)
        {
            //level1 or above
        }
        else
        {
            //level0
            size_t N = meta_.ssTableNum;
            off_t offset = meta_.lastSSTable;
            for(size_t i = 0; i < N; i++, offset -= sizeof(SSTableMeta))
            {
                SSTableMeta sstMeta;
                if(!fp.Read(&sstMeta, offset, sizeof(SSTableMeta)))
                {
                    return false;
                }

                if(sstMeta.minKey > key || sstMeta.maxKey < key)
                {
                    continue;
                }
                
                SSTable sst(sstMeta); 
                if(sst.SearchFromDisk(key, value))
                {
                    return true;
                }
            }
        }

        return false;
    }
        

}

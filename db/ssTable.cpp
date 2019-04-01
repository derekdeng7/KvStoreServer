#include "ssTable.hpp"
#include "fileOperator.hpp"

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cmath>

namespace KvStoreServer{

    bool SSTable::SearchFromDisk(const KeyType& key, ValueType& value)
    {
        FileOperator fp("rb", (const char*)meta_.filePath);

        //read index
        if(!fp.Read(&indexArray_, sizeof(SSTableMeta), sizeof(Index) * INDEXNUM))
        {
            return false;
        }

        //search index
        size_t i = 1;
        while(key >= indexArray_[i].internalKey)
        {
            i++;
        }

        //read entries
        off_t offset = indexArray_[i - 1].offset;
        while(offset != indexArray_[i].offset)
        {
            Entry entry;
            if(!fp.Read(&entry, offset, sizeof(Entry)))
            {
                return false;
            }

            if(entry.internalKey == key)
            {
                value = entry.value;
                return true;
            }

            offset += sizeof(Entry);
        }

        return false; 
    }

    //only call when the immuable memtable flush into disk
    bool SSTable::WriteInDisk()
    {
        meta_.entryNum = entryVec_.size();
        meta_.minKey = entryVec_.begin()->internalKey;
        meta_.maxKey = entryVec_.rbegin()->internalKey;
        
        bzero(meta_.filePath, PATHLENGTH); 
        SeqType st;
        std::string str = ".sst/" + std::to_string(st.seq)  + ".db";
        strcpy(meta_.filePath, str.c_str());
      
        FileOperator fp("w+", (const char*)meta_.filePath);
        
        //write ssMeta first
        fp.Write(&meta_, 0, sizeof(SSTableMeta));

        //write index
        const size_t blockSize = ceil(entryVec_.size() / INDEXNUM);
        off_t offset = sizeof(SSTableMeta) + sizeof(Index) * INDEXNUM;
        for(size_t i = 0; i < INDEXNUM; i++)
        {
            indexArray_[i] = Index(entryVec_[blockSize * i].internalKey, offset + sizeof(Entry) * blockSize * i);
        }

        if(!fp.Write(&indexArray_, sizeof(SSTableMeta), sizeof(Index) * INDEXNUM))
        {
            return false;
        }

        //write entries
        for(auto i = entryVec_.begin(); i != entryVec_.end(); i++)
        {
            Entry entry = *i;
            if(!fp.Write(&entry, offset, sizeof(Entry)))
            {
                return false;
            }

            offset += sizeof(entry);
        }

        return true;
    }

    std::vector<Entry> SSTable::GetData()
    {
        if(entryVec_.size())
        {
            return entryVec_;
        }

        //read from disk
        FileOperator fp("rb", (const char*)meta_.filePath);

        //read entries
        Entry entry;
        off_t offset = sizeof(SSTableMeta) + sizeof(Index) * INDEXNUM;
        while(fp.Read(&entry, offset, sizeof(Entry)))
        {
            entryVec_.push_back(entry);
            offset += sizeof(Entry);
        }

        return entryVec_; 
    }

}

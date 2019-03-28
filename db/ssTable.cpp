#include "ssTable.hpp"
#include "fileOperator.hpp"

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cassert>

namespace KvStoreServer{

    bool SSTable::SearchFromDisk(const KeyType& key, ValueType& value)
    {
        FileOperator fp("rb", (const char*)meta_.filePath);

        //read entries
        Entry entry;
        off_t offset = sizeof(SSTableMeta);
        while(fp.Read(&entry, offset, sizeof(Entry)))
        {
            entryVec_.push_back(entry);
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
        
        bzero(meta_.filePath, 32); 
        SeqType st;
        std::string str = ".sst/" + std::to_string(st.seq)  + ".sdb";
        assert(str.size() == 25);
        strcpy(meta_.filePath, str.c_str());
      
        FileOperator fp("w+", (const char*)meta_.filePath);
        
        //write ssMeta first
        fp.Write(&meta_, 0, sizeof(SSTableMeta));

        //write entries
        off_t offset = sizeof(SSTableMeta);
        for(auto i = entryVec_.begin(); i != entryVec_.end(); i++)
        {
            Entry entry = *i;
            fp.Write(&entry, offset, sizeof(Entry));
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
        off_t offset = sizeof(SSTableMeta);
        while(fp.Read(&entry, offset, sizeof(Entry)))
        {
            entryVec_.push_back(entry);
            offset += sizeof(Entry);
        }

        return entryVec_; 
    }

}

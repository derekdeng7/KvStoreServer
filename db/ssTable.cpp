#include "ssTable.hpp"

#include <algorithm>
#include <iostream>
#include <cstdio>

namespace KvStoreServer{

    //only call when the immuable memtable flush into disk
    void SSTable::InitFormMemTable()
    {
        meta_.entryNum = entryVec_.size();
        meta_.minKey = (std::min_element(entryVec_.begin(), entryVec_.end()))->internalKey;
        meta_.maxKey = (std::max_element(entryVec_.begin(), entryVec_.end()))->internalKey;
        FlushInDisk();
    }

    bool SSTable::LoadFromDisk()
    {
        FILE* fp = fopen("db.sst", "rb+");
        if(fp == NULL)
        {
            std::cout << "fail to open file" << std::endl;
            return false;
        }

        off_t offset = 0;
        //read ssMeta first
        int len =fread(&meta_, sizeof(SSMeta), 1, fp);
        offset += sizeof(SSMeta);
        entryVec_.clear();

        //read entries
        while(true)
        {
            Entry entry;
            fseek(fp, offset, SEEK_SET);
            len = fread(&entry, sizeof(Entry), 1, fp);
            if(len == -1)
            {
                perror("file read error");
                fclose(fp);
                return false;
            }
            else if(len == 0)
            {
                perror("file read over");
                fclose(fp);
                return true;
            }
            else
            {
                entryVec_.push_back(entry);
            }
            offset += sizeof(Entry);
        }
       
    }

    bool SSTable::MapRead(const KeyType& key)
    {
        return true;
    }

    void SSTable::ShowData() const
    {
        std::cout << meta_.entryNum << std::endl;
        std::cout << meta_.minKey.key << std::endl;
        std::cout << meta_.maxKey.key << std::endl;

        for(auto i : entryVec_)
        {
            std::cout << i.internalKey.key << ":" << i.internalKey.seqNum << ":" << i.value.str << " ";
        }
        std::cout << std::endl;
    }

    //only call when the immuable memtable flush into disk
    bool SSTable::FlushInDisk()
    {
        FILE* fp = fopen("db.sst", "w+");
        if(fp == NULL)
        {
            std::cout << "fail to open file" << std::endl;
            return false;
        }

        off_t offset = 0;

        //write ssMeta first
        fwrite(&meta_, sizeof(meta_), 1, fp);

        //write entries
        offset += sizeof(meta_);
        for(auto i = entryVec_.begin(); i != entryVec_.end(); i++)
        {
            Entry entry = *i;
            fseek(fp, offset, SEEK_SET);
            fwrite(&entry, sizeof(Entry), 1, fp);
            offset += sizeof(entry);
        }

        fclose(fp);

        return true;
    }
}

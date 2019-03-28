#include "compaction.hpp"
#include "ssTable.hpp"

#include <iostream>

namespace KvStoreServer{

    bool Compaction::MinorCompaction(std::unique_ptr<MemTable> immuTable)
    {
        assert(immuTable->GetEntryNum() == MAXENTRYNUM);

        //do MajorCompaction before MinorCompaction
        if(!MajorCompaction(0))
        {
            return false;
        }
        
        LevelMeta levMeta0;
        std::vector<Entry> entryVec = immuTable->PopAllEntries();

        if(!fp_.Read(&levMeta0, sizeof(LsmTreeMeta), sizeof(LevelMeta)))
        {
            return false;
        }

        //dump immuTable in level0
        SSTable newTable(std::move(entryVec));
        newTable.WriteInDisk();

        return UpdateMeta(newTable.GetMeta());
    }

    bool Compaction::MajorCompaction(size_t levelNo)
    {
        if(levelNo >= levelNum_)
        {
            return true;
        }

        LevelMeta levMeta(levelNo);

        //read level n
        const off_t levOffset = sizeof(LsmTreeMeta) + sizeof(LevelMeta) * levelNo;
        if(!fp_.Read(&levMeta, levOffset, sizeof(LevelMeta)))
        {
            return false;
        }

        if(!levelNo && levMeta.ssTableNum < 4)
        {
            //don't need to do MajorCompaction
            return true;
        }

        if(levelNo && levMeta.entryNum < levMeta.maxEntryNum)
        {
            //don't need to do MajorCompaction
            return true;
        }

        KeyType minKey = levMeta.minKey;
        KeyType maxKey = levMeta.maxKey;
        off_t insertOffset;

        //read level n+1
        LevelMeta nextLevMeta(levelNo + 1);
        if(!fp_.Read(&nextLevMeta, levOffset + sizeof(LevelMeta), sizeof(LevelMeta)))
        {
            return false;
        }

        //read level n SSTableMeta
        std::vector<SSTableMeta> sstMetaVec;
        if(!GetCurLevelSSTMeta(levMeta, sstMetaVec))
        {
            return false;
        }

        //read level n+1 overlap SSTableMeta
        if(levelNo > 0)
        {
            minKey = (sstMetaVec.begin())->minKey;
            maxKey = (sstMetaVec.rbegin())->maxKey;
        }

        if(!GetOverlapSSTMeta(nextLevMeta, minKey, maxKey, sstMetaVec, insertOffset))
        {
            perror("fail to GetOverlapSSTMeta");
            return false;
        }
        
        std::deque<Entry> entryDeq = MinHeapSort(std::move(sstMetaVec));
        std::vector<SSTableMeta> newSSTMetaVec = WriteInDisk(entryDeq);

        //update nextLevelMeta and SSTableMeta
        if(!UpdateMeta(nextLevMeta, insertOffset, newSSTMetaVec))
        {
            perror("fail to UpdateMeta in level0");
            return false;
        }
        
        return MajorCompaction(levelNo + 1);
    }

    bool Compaction::GetCurLevelSSTMeta(LevelMeta& levMeta, std::vector<SSTableMeta>& sstMetaVec)
    {
        //read the first N ssTable
        size_t N = (levMeta.entryNum - levMeta.maxEntryNum) / MAXENTRYNUM + 1;
        size_t sstMetaOffset = levMeta.header.next;
        SSTableMeta sstMeta;
        for(size_t i = 0; i < N; i++)
        {
            if(!fp_.Read(&sstMeta, sstMetaOffset, sizeof(SSTableMeta)))
            {
                return false;
            }

            levMeta.entryNum -= sstMeta.entryNum;
            sstMetaVec.push_back(sstMeta);
            sstMetaOffset = sstMeta.next;
        }

        if(levMeta.levelNo == 0)
        {
            //update level 0
            LevelMeta newMeta;
            if(!fp_.Write(&newMeta, sizeof(LsmTreeMeta), sizeof(LevelMeta)))
            {
                return false;
            }
            return true;
        }

        //update level n
        levMeta.ssTableNum -= N;
        levMeta.header.next = sstMetaOffset;

        if(!fp_.Read(&sstMeta, sstMetaOffset, sizeof(SSTableMeta)))
        {
            return false;
        }

        levMeta.minKey = sstMeta.minKey;
        const off_t levelOffset = sizeof(LsmTreeMeta) + sizeof(LevelMeta) * levMeta.levelNo;
        if(!fp_.Write(&levMeta, levelOffset, sizeof(LevelMeta)))
        {
            return false;
        }

        return true;
    }

    bool Compaction::GetOverlapSSTMeta(LevelMeta& levMeta, const KeyType& minKey, const KeyType& maxKey, 
        std::vector<SSTableMeta>& sstMetaVec, off_t& insertOffset)
    {
        assert(maxKey > minKey);

        //get header offset
        const off_t headerOffset = sizeof(LsmTreeMeta) + sizeof(LevelMeta) * (levMeta.levelNo + 1) - sizeof(SSTableMeta);
        insertOffset = headerOffset;
      
        if(levMeta.ssTableNum == 0)
        {
            levelNum_++;
            return true;
        }

        SSTableMeta sstMeta = levMeta.header;
        SSTableMeta nextMeta;

        while(sstMeta.next != 0)
        {
            off_t nextOffset = sstMeta.next;
            if(!fp_.Read(&nextMeta, nextOffset, sizeof(SSTableMeta)))
            {
                return false;
            }

            if(minKey > nextMeta.maxKey)
            {
                sstMeta = nextMeta;
                insertOffset = nextOffset;
            }
            else if(maxKey < nextMeta.minKey)
            {
                break;
            }
            else
            {
                sstMeta.next = nextMeta.next;
                sstMetaVec.push_back(nextMeta);

                levMeta.entryNum -= nextMeta.entryNum;
                levMeta.ssTableNum--;
            }
        }

        if(!fp_.Write(&sstMeta, insertOffset, sizeof(SSTableMeta)))
        {
            return false;
        }

        //update levMeta.minKey and levMeta.maxKey
        if(sstMeta.next == 0)
        {
            //sstMeta becomes the last SSTableMeta
            levMeta.maxKey = sstMeta.maxKey;
        }

        if(insertOffset == headerOffset)
        {
            levMeta.header.next = sstMeta.next;
            if(!fp_.Write(&levMeta.header, headerOffset, sizeof(SSTableMeta)))
            {
                return false;
            }

            if(levMeta.ssTableNum == 0)
            {
                levMeta.minKey = levMeta.header.minKey;
                return true;
            }

            SSTableMeta newFirstSSTMeta;
            if(!fp_.Read(&newFirstSSTMeta, levMeta.header.next, sizeof(SSTableMeta)))
            {
                return false;
            }
            levMeta.minKey = newFirstSSTMeta.minKey;
        }

        return true;
    }

    std::deque<Entry> Compaction::MinHeapSort(std::vector<SSTableMeta> sstMetaVec)
    {
        assert(sstMetaVec.size());

        std::vector<std::vector<Entry>> entryVecVec;
        std::deque<Entry> result;
        std::priority_queue<MinHeapNode, std::vector<MinHeapNode>, NodeCmp> pQueue;

        //read all ssTable into memory
        for(auto iter : sstMetaVec)
        {
            SSTable sst(iter);
            entryVecVec.push_back(sst.GetData());
        }

        //sort all entries
        for(size_t i = 0; i < entryVecVec.size(); i++)
        {
            pQueue.push(MinHeapNode(entryVecVec[i][0], i, 1));
        }

        while(pQueue.size())
        {
            MinHeapNode tmpNode = pQueue.top();
            pQueue.pop();
            
            if(result.size() && result.back().internalKey == tmpNode.entry.internalKey)
            {
                result.pop_back();
            }

            result.push_back(tmpNode.entry);
            if(tmpNode.entryNo < entryVecVec[tmpNode.vecNo].size())
            {
                tmpNode.entry = entryVecVec[tmpNode.vecNo][tmpNode.entryNo];
                tmpNode.entryNo++;
                pQueue.push(tmpNode);
            }
        }

        for(auto iter : sstMetaVec)
        {
            if(remove((const char*)iter.filePath))
            {
                perror("fail to remove .sdb");
            }
        }

        return result;
    }

    std::vector<SSTableMeta> Compaction::WriteInDisk(std::deque<Entry>& entryDeq)
    {
        assert(entryDeq.size());

        std::vector<SSTableMeta> result;
        while(entryDeq.size())
        {
            std::vector<Entry> entryVec;
            for(auto i = 0; i < MAXENTRYNUM && entryDeq.size(); i++)
            {
                entryVec.push_back(entryDeq.front());
                entryDeq.pop_front();
            }

            SSTable sst(std::move(entryVec));
            sst.WriteInDisk();
            result.push_back(sst.GetMeta());
        }

        return result;
    }

    //use in MinorCompaction
    bool Compaction::UpdateMeta(SSTableMeta sstMeta)
    {
        LsmTreeMeta lsmMeta;
        LevelMeta levMeta;

        //read Metas
        if(!fp_.Read(&lsmMeta, 0, sizeof(LsmTreeMeta)))
        {
          return false;
        }

        off_t levelOffset = sizeof(LsmTreeMeta);
        if(!fp_.Read(&levMeta, levelOffset, sizeof(LevelMeta)))
        {
          return false;
        }

        //update LevelMeta
        levMeta.ssTableNum++;
        levMeta.entryNum += sstMeta.entryNum;
        if(levMeta.minKey > sstMeta.minKey)
        {
            levMeta.minKey = sstMeta.minKey;
        }

        if(levMeta.maxKey < sstMeta.maxKey)
        {
            levMeta.maxKey = sstMeta.maxKey;
        }

        //insert in head of list
        sstMeta.next = levMeta.header.next;
        levMeta.header.next = lsmMeta.slot;
        
        if(!fp_.Write(&levMeta, levelOffset, sizeof(LevelMeta)))
        {
            return false;
        }

        //add SSTableMeta 
        if(!fp_.Write(&sstMeta, lsmMeta.slot, sizeof(SSTableMeta)))
        {
            return false;
        }

        //update LsmTreeMeta
        lsmMeta.ssTableNum++;
        lsmMeta.entryNum += sstMeta.entryNum;
        lsmMeta.slot += sizeof(SSTableMeta);
        return fp_.Write(&lsmMeta, 0, sizeof(LsmTreeMeta));
    }

    //use in MajorCompaction
    bool Compaction::UpdateMeta(LevelMeta nextLevMeta, off_t insertOffset, std::vector<SSTableMeta> sstMetaVec)
    {
        LsmTreeMeta lsmMeta;
        if(!fp_.Read(&lsmMeta, 0, sizeof(LsmTreeMeta)))
        {
          return false;
        }

        SSTableMeta sstMeta;
        if(!fp_.Read(&sstMeta, insertOffset, sizeof(SSTableMeta)))
        {
          return false;
        }
        sstMetaVec.rbegin()->next = sstMeta.next;
        sstMeta.next = lsmMeta.slot;

        //write SSTableMeta (expect the last one) in .Manifest
        size_t i = 0;
        for(; i < sstMetaVec.size() - 1; i++)
        {
            nextLevMeta.entryNum += sstMetaVec[i].entryNum;
            sstMetaVec[i].next = lsmMeta.slot + sizeof(SSTableMeta);

            if(!fp_.Write(&sstMetaVec[i], lsmMeta.slot, sizeof(SSTableMeta)))
            {
              return false;
            }
            lsmMeta.slot += sizeof(SSTableMeta);
        }

        //write the last SSTableMeta in .Manifest
        if(!fp_.Write(&sstMetaVec[i], lsmMeta.slot, sizeof(SSTableMeta)))
        {
          return false;
        }
            
        //update level n+1 LevelMeta
        nextLevMeta.ssTableNum += sstMetaVec.size();
        nextLevMeta.entryNum += sstMetaVec.rbegin()->entryNum;

        if(nextLevMeta.minKey > sstMetaVec.begin()->minKey)
        {
            nextLevMeta.minKey = sstMetaVec.begin()->minKey;
        }

        if(nextLevMeta.maxKey < sstMetaVec.rbegin()->maxKey)
        {
            nextLevMeta.maxKey = sstMetaVec.rbegin()->maxKey;
        }

        if(!fp_.Write(&nextLevMeta, sizeof(LsmTreeMeta) + sizeof(LevelMeta) * nextLevMeta.levelNo, sizeof(LevelMeta)))
        {
            return false;
        }

        //update insert SSTableMeta
        if(!fp_.Write(&sstMeta, insertOffset, sizeof(SSTableMeta)))
        {
          return false;
        }

        //update LsmTreeMeta
        lsmMeta.levelNum = levelNum_;
        lsmMeta.ssTableNum += sstMetaVec.size();
        lsmMeta.slot += sizeof(SSTableMeta);
        
        return fp_.Write(&lsmMeta, 0, sizeof(LsmTreeMeta));
    }

}

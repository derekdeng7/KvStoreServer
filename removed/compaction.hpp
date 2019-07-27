#ifndef _KVSTORESERVER_DB_COMPACTION_HPP_
#define _KVSTORESERVER_DB_COMPACTION_HPP_

#include "../include/fileOperator.hpp"
#include "memTable.hpp"

#include <queue>
#include <queue>
#include <thread>
#include <vector>

namespace KvStoreServer{

    struct MinHeapNode
    {
        Entry entry;
        size_t vecNo;
        size_t entryNo;

        MinHeapNode(Entry e, size_t vn, size_t en)
          : entry(e), vecNo(vn), entryNo(en)
        {}

    };

    struct NodeCmp
    {
        bool operator()(const MinHeapNode& leftNode, const MinHeapNode& rightNode) const 
        {
            return leftNode.entry > rightNode.entry;
        }
    };

    class Compaction
    {
    public:
        Compaction(size_t levelNum)
          : fp_("rb+"), levelNum_(levelNum)
        {}

        bool MinorCompaction(std::unique_ptr<MemTable> immuTable);

    private:
        bool MajorCompaction(size_t levelNo);

        bool GetCurLevelSSTMeta(LevelMeta& levMeta, std::vector<SSTableMeta>& sstMetaVec);
        
        bool GetOverlapSSTMeta(LevelMeta& levMeta, const KeyType& minKey, const KeyType& maxKey, 
            std::vector<SSTableMeta>& sstMetaVec, off_t& insertOffset);
        
        std::deque<Entry> MinHeapSort(const std::vector<SSTableMeta>& sstMetaVec);
        std::vector<SSTableMeta> WriteInDisk(std::deque<Entry>& entryDeq);

        bool UpdateMeta(SSTableMeta sstMeta);
        bool UpdateMeta(LevelMeta nextLevMeta, off_t insertOffset, std::vector<SSTableMeta> sstMetaVec);

        bool RemoveOldSSTable(const std::vector<SSTableMeta>& sstMetaVec);

        FileOperator fp_;
        size_t levelNum_;
    };

}
#endif //_KVSTORESERVER_DB_COMPACTION_HPP_

#ifndef _KVSTORESERVER_DB_SKIPLIST_HPP_
#define _KVSTORESERVER_DB_SKIPLIST_HPP_

#include "../include/atomicPointer.hpp"
#include "../include/base.hpp"
#include "../include/random.hpp"

#include <cstdlib>
#include <vector>
#include <stack>

namespace KvStoreServer{

    class Node
    { 
    public:
        Node() : pNext_(nullptr), pDown_(nullptr) 
        {}

        Node(const KeyType& key, const ValueType& value) : entry_(key, value), pNext_(nullptr), pDown_(nullptr) 
        {}

        Node* Next() const
        {
            return reinterpret_cast<Node*>(pNext_.AcquireLoad());
        }

        void SetNext(Node* node)
        {
            pNext_.ReleaseStore(node);
        }

        Node* Down() const
        {
            return reinterpret_cast<Node*>(pDown_.AcquireLoad());
        }

        void SetDown(Node* node)
        {
            pDown_.ReleaseStore(node);
        }

        Entry GetEntry() const
        {
            return entry_;
        }

        KeyType GetKey() const
        {
            return entry_.internalKey;
        }

        ValueType GetValue() const
        {
            return entry_.value.str;
        }

    private:
        Entry entry_;
        AtomicPointer pNext_;
        AtomicPointer pDown_;
    };

    class SkipList
    {
    public:
        SkipList(const size_t maxHeight);

        bool Search(const KeyType& key, ValueType& value);
        void Insert(const Entry& entry);        
        void ShowData() const;
        std::vector<Entry> PopAllEntries();

        size_t GetHeight() const 
        { 
            return height_; 
        }

        size_t GetEntryNum() const 
        { 
            return entryNum_;
        }

    private:
        std::stack<Node*> FindGreaterOrEqual(const KeyType& key);
        int GetRandomHeight();

        Node* header_;
        const size_t maxHeight_;
        size_t height_;
        size_t entryNum_;
        Random random_;
    };
}

#endif //_KVSTORESERVER_DB_SKIPLIST_HPP_

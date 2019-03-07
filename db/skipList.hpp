#ifndef _KVSTORESERVER_DB_SKIPLIST_HPP_
#define _KVSTORESERVER_DB_SKIPLIST_HPP_

#include "atomicPointer.hpp"
#include "entry.hpp"
#include "random.hpp"

#include <cstdlib>
#include <vector>
#include <stack>

namespace KvStoreServer{

    constexpr int MAX_LEVEL = 15;

    class Node
    { 
    public:
        Node() //: pNext_(new AtomicPointer(nullptr)), pDown_(nullptr) 
        {}

        Node(const KeyType& key, const ValueType& value) : entry_(key, value)
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
            return entry_.key;
        }
        
        ValueType GetValue() const
        {
            return entry_.value.str;
        }

        void SetValue(const ValueType& v)
        {
            entry_.value = v;
        }

        void Delete()
        {
            entry_.isDeleted = true; 
        }

        bool IsDeleted() const
        {
            return entry_.isDeleted; 
        }

    private:
        Entry entry_;
        AtomicPointer pNext_;
        AtomicPointer pDown_;
    };

    class SkipList
    {
    public:
        SkipList();
        bool Search(const KeyType& key, ValueType& value);
        void Insert(const KeyType& key, const ValueType& value);        
        void Delete(const KeyType& key);
        bool Remove(const KeyType& key);
        void ShowData() const;
        std::vector<Entry> PopAllEntries();

        size_t GetLevelNum() const 
        { 
            return levelNum_; 
        }

        size_t GetEntryNum() const 
        { 
            return entryNum_;
        }


    private:
        bool FindGreaterOrEqual(const KeyType& key, std::stack<Node*>& updateStack);
        int GetRandomHeight();

        Node* header_;
        size_t levelNum_;
        size_t entryNum_;
        Random random_;
    };
}

#endif //_KVSTORESERVER_DB_SKIPLIST_HPP_

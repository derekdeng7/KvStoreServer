#include "skipList.hpp"

#include <iostream>
#include <cassert>
#include <string>

namespace KvStoreServer{

    
    SkipList::SkipList(const size_t maxHeight)
      : header_(new Node()), maxHeight_(maxHeight), height_(1), entryNum_(0), random_(0xdeadbeef)
    {}
  

    bool SkipList::Search(const KeyType& key, ValueType& value)
    {
        if(entryNum_ == 0)
        {
            return false;
        }

        std::stack<Node*> updateStack = FindGreaterOrEqual(key);

        Node* cursor = updateStack.top();
        if(cursor->Next() == nullptr)
        {
            return false;
        }

        Entry entry = cursor->Next()->GetEntry();
        value = entry.value;

        return entry.internalKey == key;
    }

    void SkipList::Insert(const KeyType& key, const ValueType& value)
    {

        Node* cursor = header_;
        //if the skipList is empty
        if(header_->Next() == nullptr)
        {
            Node* newNode = new Node(key, value);
            header_->SetNext(newNode);
            entryNum_++;
            return;
        }

        //store the nodes need to link to the newNode 
        std::stack<Node*> updateStack = FindGreaterOrEqual(key);

        Node* tmpNode = nullptr;
        int height = GetRandomHeight();

        for(int i = height; i > 0; i--)
        {
            if(updateStack.size())
            {
                cursor = updateStack.top();
                updateStack.pop();
            }
            //create a new level in the top
            else
            {
                Node* newHeader = new Node();
                newHeader->SetDown(header_);
                header_ = newHeader;
                height_++;
                cursor = header_;
            }
            Node* newNode = new Node(key, value);
            newNode->SetNext(cursor->Next());
            cursor->SetNext(newNode); 
            newNode->SetDown(tmpNode);
            tmpNode = newNode;
        }

        entryNum_++;
    }
    
    void SkipList::ShowData() const
    {
        Node* cursor = header_;
        while(cursor != nullptr)
        {
            Node* cursorHeader = cursor;
            cursor = cursorHeader->Next();
            
            while(cursor != nullptr)
            {
                Entry entry = cursor->GetEntry();
                std::cout << entry.internalKey.key << ":" << entry.value.str << " ";
                cursor = cursor->Next();
            }
            std::cout << std::endl;
            cursor = cursorHeader->Down();
        }
        std::cout << "finish!" << std::endl;
    }

    std::vector<Entry> SkipList::PopAllEntries()
    {
        std::vector<Entry> result;
        Node* cursor = header_;
        //go to the bottom level
        while(header_->Down() != nullptr)
        {
            //delete all the node expect those at the bottom level
            header_ = header_->Down();
            while(cursor->Next() != nullptr)
            {
                Node* delNode = cursor->Next();
                cursor->SetNext(delNode->Next());
                delete delNode;
            }
            delete cursor;
            cursor = header_;
        }

        while(cursor->Next() != nullptr)
        {
            Node* delNode = cursor->Next();
            result.push_back(delNode->GetEntry());
            cursor->SetNext(delNode->Next());
            delete delNode;
        }

        return result;
    }
    
    std::stack<Node*> SkipList::FindGreaterOrEqual(const KeyType& key)
    {
        Node* cursor = header_;
        int searchCount = 0;

        std::stack<Node*> updateStack;

        while(cursor != nullptr)
        {
            searchCount++;
            if(cursor->Next() == nullptr)
            {
                updateStack.push(cursor);
                cursor = cursor->Down();
            }
            else if(cursor->Next()->GetKey() >= key)
            {
                updateStack.push(cursor);
                cursor = cursor->Down();
            }
            else
            {
                cursor = cursor->Next();
            }
        }
        return updateStack;
    }

    int SkipList::GetRandomHeight()
    {
        size_t height = 1;
        static const unsigned int brandching = 4;
        while(height < maxHeight_ && ((random_.Next() % brandching) == 0))
        {
            height++;
        }
        
        assert(height >0);
        assert(height <= maxHeight_);
        
        return height;
    }

}

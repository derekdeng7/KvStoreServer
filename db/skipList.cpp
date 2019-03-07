#include "skipList.hpp"

#include <iostream>
#include <cassert>
#include <string>

namespace KvStoreServer{

    
    SkipList::SkipList()
      : header_(new Node()), levelNum_(1), entryNum_(0), random_(0xdeadbeef)
    {}
  

    bool SkipList::Search(const KeyType& key, ValueType& value)
    {
        std::stack<Node*> updateStack;
        if(FindGreaterOrEqual(key, updateStack))
        {
            Node* cursor = updateStack.top();
            value = cursor->GetValue();
            return true;
        }

        return false;
    }

    void SkipList::Insert(const KeyType& key, const ValueType& value)
    {

        Node* cursor = header_;
        //if the skipList is empty
        if(header_->Next() == nullptr)
        {
            Node* newNode = new Node(key, value);
            header_->SetNext(newNode);
            return;
        }

        //store the nodes need to link to the newNode 
        std::stack<Node*> updateStack;
        //the key has already existed, update
        if(FindGreaterOrEqual(key, updateStack))
        {
            //we just update the bottom level
            Node* cursor = updateStack.top();
            cursor->SetValue(value);
            return;
        }

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
                levelNum_++;
                cursor = header_;
            }
            Node* newNode = new Node(key, value);
            newNode->SetNext(cursor->Next());
            cursor->SetNext(newNode); 
            newNode->SetDown(tmpNode);
            tmpNode = newNode;
        }

    }
    
    void SkipList::Delete(const KeyType& key)
    {
        std::stack<Node*> updateStack;
        if(FindGreaterOrEqual(key, updateStack))
        {
            Node* cursor = updateStack.top();
            cursor->Delete();
        }
    }

    //not allow to be used in db
    bool SkipList::Remove(const KeyType& key)
    {        
        Node* cursor = header_;
        while(cursor != nullptr)
        {
            if(cursor->Next() == nullptr)
            {
                cursor = cursor->Down();
            }
            else if(key < cursor->Next()->GetKey())
            {
                cursor = cursor->Down();
            }
            else if(key > cursor->Next()->GetKey())
            {
                cursor = cursor->Next();
            }
            //find the target node
            else
            {
                Node* delNode = cursor->Next();
                cursor->SetNext(delNode->Next());
                delete delNode;
                //reach the lowest level;
                if(cursor->Down() == nullptr)
                {
                    return true;
                }
                cursor = cursor->Down();
                //this level is empty
                if(header_->Next() == nullptr)
                {
                    delete header_;
                    header_ = cursor;
                }
            }            
        }
        
        return false;
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
                if(!cursor->IsDeleted())
                {
                    std::cout << cursor->GetKey() << ":" << cursor->GetValue().str << " ";
                }
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
        while(cursor->Down() != nullptr)
        {
            //delete all the node expect those at the bottom level
            cursor = cursor->Down();
            while(cursor->Next() != nullptr)
            {
                Node* delNode = cursor->Next();
                cursor->SetNext(delNode->Next());
                delete delNode;
            }
            delete header_;
            header_ = cursor;
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
    
    bool SkipList::FindGreaterOrEqual(const KeyType& key, std::stack<Node*>& updateStack)
    {
        Node* cursor = header_;
        int searchCount = 0;

        while(cursor != nullptr)
        {
            searchCount++;
            if(cursor->Next() == nullptr)
            {
                updateStack.push(cursor);
                cursor = cursor->Down();
            }
            else if(key < cursor->Next()->GetKey())
            {
                updateStack.push(cursor);
                cursor = cursor->Down();
            }
            else if(key > cursor->Next()->GetKey())
            {
                cursor = cursor->Next();
            }
            //the node has already exists
            else
            {
                cursor = cursor->Next();
                //reach the bottom level
                while(cursor->Down() != nullptr)
                {
                    cursor = cursor->Down();
                }
                //value = cursor->Next()->GetVaule();
                //if exists, just push the bottom one into stack
                updateStack.push(cursor);
                std::cout << "searchCount: " << searchCount <<std::endl;
                return true;
            }            
        }

        return false;
    }

    int SkipList::GetRandomHeight()
    {
        int height = 1;
        static const unsigned int brandching = 4;
        while(height < MAX_LEVEL && ((random_.Next() % brandching) == 0))
        {
            height++;
        }
        
        assert(height >0);
        assert(height <= MAX_LEVEL);
        
        return height;
    }

}

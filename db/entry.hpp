#ifndef _KVSTORESERVER_DB_ENTRY_HPP_
#define _KVSTORESERVER_DB_ENTRY_HPP_

#include <cstring>
#include <chrono>

namespace KvStoreServer{

    struct KeyType
    {
        size_t key;
        size_t seqNum;
        
        KeyType()
        {
            std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>
              (std::chrono::system_clock::now().time_since_epoch());
            seqNum = static_cast<size_t>(ms.count());
        }

        KeyType(const size_t k) : key(k)
        {
            std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>
              (std::chrono::system_clock::now().time_since_epoch());
            seqNum = static_cast<size_t>(ms.count());
        }

        KeyType(const KeyType& kt)
          : key(kt.key), seqNum(kt.seqNum)
        {}

        bool operator>(const KeyType& kt)
        {
            return this->key > kt.key;
        }

        bool operator<(const KeyType& kt)
        {
            return this->key < kt.key;
        }
        
        bool operator==(const KeyType& kt)
        {
            return this->key == kt.key;
        }

        bool operator>=(const KeyType& kt)
        {
            return this->key >= kt.key;
        }
    };
    
    struct ValueType 
    {
        char str[16];
    
        ValueType(const char* s = "")
        {
            bzero(str, sizeof(str));
            strcpy(str, s);
        }

        ValueType(const ValueType& vt)
        {
            bzero(str, sizeof(str));
            strcpy(str, vt.str);
        }
    };

    struct Entry
    {
        KeyType internalKey;
        ValueType value;
        
        Entry() 
        {}

        Entry(const Entry& e) 
          : internalKey(e.internalKey), value(e.value)
        {}

        Entry(const KeyType& ik, const ValueType& v)
          : internalKey(ik), value(v)
        {}

        bool operator>(const Entry& e)
        {
            return this->internalKey > e.internalKey;
        }

        bool operator<(const Entry& e)
        {
            return this->internalKey < e.internalKey;
        }

        bool operator==(const Entry& e)
        {
            return this->internalKey == e.internalKey;
        }
    };
}

#endif //_KVSTORESERVER_DB_ENTRY_HPP_

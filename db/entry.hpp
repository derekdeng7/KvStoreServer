#ifndef _KVSTORESERVER_DB_ENTRY_HPP_
#define _KVSTORESERVER_DB_ENTRY_HPP_

#include <cassert>
#include <cstdio>
#include <chrono>
#include <cstring>

namespace KvStoreServer{

    constexpr auto MANIFESTPATH = ".Manifest";
    constexpr auto DELETETAG = "ENTRYHASBEENDEL";
    constexpr auto MAXHEIGHT = 16;
    constexpr auto MAXENTRYNUM = 2048;
    constexpr auto MAXHLEVELNUM = 7;

    struct SeqType
    {
        size_t seqNum;

        SeqType()
        {
            std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>
              (std::chrono::system_clock::now().time_since_epoch());
            seqNum = static_cast<size_t>(ms.count());
        }

    };

    struct KeyType
    {
        size_t key;
        SeqType seqNum;
        
        KeyType()
          :key(0)
        {}

        KeyType(const size_t k) : key(k)
        {}

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
    };

    struct LsmTreeMeta
    {
        size_t levelNum;
        size_t ssTableNum;
        size_t entryNum;
        off_t slot;

        LsmTreeMeta() 
          : levelNum(1), ssTableNum(0), entryNum(0), slot(0)
        {}
    };

    struct LevelMeta
    {
        size_t ssTableNum;
        KeyType minKey;
        KeyType maxKey;
        off_t firstSSTable;
        off_t lastSSTable;

        LevelMeta()
          : ssTableNum(0), firstSSTable(0), lastSSTable(0)
        {}
    };

    struct SSTableMeta
    {
        char filePath[32];
        size_t entryNum;
        KeyType minKey;
        KeyType maxKey;
        off_t prev;
        off_t next;

        SSTableMeta()
          : entryNum(0), prev(0), next(0)
        {
            bzero(filePath, sizeof(filePath));
        }
    };
}

#endif //_KVSTORESERVER_DB_ENTRY_HPP_

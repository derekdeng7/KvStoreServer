#ifndef _KVSTORESERVER_DB_BASE_HPP_
#define _KVSTORESERVER_DB_BASE_HPP_

#include <cassert>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <cstring>
#include <limits>

namespace KvStoreServer{

    const int MAXEVENTNUM = 1024;
    const int MAXFDNUM = 1024;
    const int BUFSIZE = 1024;
    const int MESSAGESIZE = 1024;

    constexpr auto MANIFESTPATH = ".Manifest";
    constexpr auto LOGMETAPATH = ".LogMeta";
    constexpr auto DELETETAG = "ENTRYHASBEENDEL";

    constexpr size_t MAXHEIGHT = 16;
    constexpr size_t MAXENTRYNUM = 65536;
    constexpr size_t MAXHLEVELNUM = 7;
    constexpr size_t MAXSYNCQUEUESIZE = 4096;

    constexpr int PATHLENGTH = 32;
    constexpr size_t INDEXNUM = 32;


    struct SeqType
    {
        size_t seq;

        SeqType()
        {
            std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>
              (std::chrono::system_clock::now().time_since_epoch());
            seq = static_cast<size_t>(ms.count());
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

        bool operator>(const KeyType& kt) const
        {
            return this->key > kt.key;
        }

        bool operator<(const KeyType& kt) const 
        { 
            return this->key < kt.key; 
        }
        
        bool operator==(const KeyType& kt) const 
        { 
            return this->key == kt.key; 
        }

        bool operator>=(const KeyType& kt) const
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

        bool operator>(const Entry& e) const
        {
            if(this->internalKey == e.internalKey)
            {
                assert(internalKey.seqNum.seq != e.internalKey.seqNum.seq);
                return internalKey.seqNum.seq > e.internalKey.seqNum.seq;
            }
            else
            {
                return this->internalKey > e.internalKey;
            }
        }

    };

    struct SSTableMeta
    {
        char filePath[PATHLENGTH];
        size_t entryNum;
        KeyType minKey;
        KeyType maxKey;
        off_t next;

        SSTableMeta()
          : entryNum(0), minKey((std::numeric_limits<size_t>::max)()), next(0)
        {
            bzero(filePath, PATHLENGTH);
        }
    };

    struct LevelMeta
    {
        size_t levelNo;
        size_t ssTableNum;
        size_t entryNum;
        KeyType minKey;
        KeyType maxKey;
        size_t maxEntryNum;
        SSTableMeta header;

        LevelMeta(size_t levNo = 0)
          : levelNo(levNo), ssTableNum(0), entryNum(0), minKey((std::numeric_limits<size_t>::max)()), 
            maxEntryNum(MAXENTRYNUM * pow(10, levelNo))
        {}
    };

    struct LogMeta
    {
        char activeLogPath[PATHLENGTH];
        char frozenLogPath[PATHLENGTH];
        size_t activeLogEntryNum;
        size_t frozenLogEntryNum;

        LogMeta()
          : activeLogEntryNum(0), frozenLogEntryNum(0)
        {
            bzero(activeLogPath, PATHLENGTH);
            bzero(frozenLogPath, PATHLENGTH);
        }
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

}

#endif //_KVSTORESERVER_DB_BASE_HPP_

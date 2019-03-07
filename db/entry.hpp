#ifndef _KVSTORESERVER_DB_ENTRY_HPP_
#define _KVSTORESERVER_DB_ENTRY_HPP_

#include <cstring>
#include <string>

namespace KvStoreServer{

    typedef size_t KeyType; 
    //typedef std::string ValueType;
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
        KeyType key;
        ValueType value;
        bool isDeleted;
        
        Entry() : isDeleted(false) {}

        Entry(const KeyType& k, const ValueType& v)
          : key(k), value(v), isDeleted(false)
        {}

    };
}

#endif //_KVSTORESERVER_DB_ENTRY_HPP_
